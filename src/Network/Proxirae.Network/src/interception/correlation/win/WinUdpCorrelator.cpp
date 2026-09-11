#include <chrono>
#include <mutex>

#include "interception/correlation/win/WinUdpCorrelator.h"
#include "interception/diversion/win/WinPacketContext.h"
#include "environment/inet.h"

namespace Proxirae {
	class WinUdpCorrelator::PendingPacketCache {
	public:
		PendingPacketCache() : m_lastGcTime(std::chrono::steady_clock::now()) {}

		void Enqueue(const FiveTuple& key, const Packet& packet, const std::function<void(IPacketContext&)>& callback) {
			std::lock_guard<std::mutex> lock(m_mutex);
			if (m_map.size() < MAX_PENDING_PACKETS) {
				m_map.emplace(key, PendingPacket{ packet, std::chrono::steady_clock::now() });
			}
			else {
				auto ctxOpt = WinPacketContext::TryCreate(packet.data.data(), packet.length, packet.metadata);
				if (ctxOpt.has_value()) {
					callback(ctxOpt.value());
				}
			}
		}

		void EvictStale(const std::function<void(IPacketContext&)>& callback) {
			auto now = std::chrono::steady_clock::now();

			std::lock_guard<std::mutex> lock(m_mutex);
			if (now - m_lastGcTime < GC_INTERVAL) return;
			m_lastGcTime = now;

			for (auto it = m_map.begin(); it != m_map.end(); ) {
				if (now - it->second.timestamp > PACKET_TIMEOUT) {
					auto ctxOpt = WinPacketContext::TryCreate(it->second.packet.data.data(), it->second.packet.length, it->second.packet.metadata);
					if (ctxOpt.has_value()) {
						callback(ctxOpt.value());
					}
					it = m_map.erase(it);
				}
				else {
					++it;
				}
			}
		}

		void ProcessMatches(const FiveTuple& searchKey, const std::function<bool(const Packet&)>& processor) {
			std::lock_guard<std::mutex> lock(m_mutex);
			auto range = m_map.equal_range(searchKey);
			for (auto& it = range.first; it != range.second; ) {
				if (processor(it->second.packet)) {
					it = m_map.erase(it);
				}
				else {
					++it;
				}
			}
		}

	private:
		struct PendingPacket {
			Packet packet;
			std::chrono::steady_clock::time_point timestamp;
		};

	private:
		std::unordered_multimap<FiveTuple, PendingPacket, FiveTupleHash> m_map;
		std::chrono::steady_clock::time_point m_lastGcTime;
		std::mutex m_mutex;

	private:
		static constexpr auto GC_INTERVAL = std::chrono::milliseconds(25);
		static constexpr auto PACKET_TIMEOUT = std::chrono::milliseconds(100);
		static constexpr size_t MAX_PENDING_PACKETS = 5000;
	};

	WinUdpCorrelator::WinUdpCorrelator(IProcessGuard& monitor, AssociationTable& associations)
		: m_guard(monitor), m_associations(associations), m_cache(std::make_unique<PendingPacketCache>()) {
	}

	WinUdpCorrelator::~WinUdpCorrelator() = default;

	bool WinUdpCorrelator::CorrelateNetwork(const Packet& packet, const std::function<void(IPacketContext&)>& callback)
	{
		auto ctxOpt = WinPacketContext::TryCreate(packet.data.data(), packet.length, packet.metadata);

		if (!ctxOpt.has_value() || !ctxOpt->IsUdp()) {
			return false;
		}

		auto& context = ctxOpt.value();

		FiveTuple fullKey{
			.srcAddress = WinDivertHelperNtohl(context.IsOutbound() ? context.GetSourceAddress() : context.GetDestinationAddress()),
			.srcPort = WinDivertHelperNtohs(context.IsOutbound() ? context.GetSourcePort() : context.GetDestinationPort()),
			.dstAddress = WinDivertHelperNtohl(context.IsOutbound() ? context.GetDestinationAddress() : context.GetSourceAddress()),
			.dstPort = WinDivertHelperNtohs(context.IsOutbound() ? context.GetDestinationPort() : context.GetSourcePort()),
			.protocol = context.GetProtocol()
		};

		FiveTuple bindKey = fullKey;
		bindKey.dstAddress = 0;
		bindKey.dstPort = 0;

		if (m_associations.AssociationExists(fullKey)) {
			context.SetProcessId(m_associations.GetAssociation(fullKey).value().processId);
			callback(context);
		}
		else if (m_associations.AssociationExists(bindKey)) {
			context.SetProcessId(m_associations.GetAssociation(bindKey).value().processId);
			callback(context);
		}
		else {
			m_cache->EvictStale(callback);
			m_cache->Enqueue(bindKey, packet, callback);
		}

		return true;
	}

	bool WinUdpCorrelator::CorrelateSocket(const PacketMetadata& metadata, const std::function<void(IPacketContext&)>& callback)
	{
		if (metadata.Socket.Protocol != IPPROTO_UDP) {
			return false;
		}

		if (metadata.Event == WINDIVERT_EVENT_SOCKET_BIND || metadata.Event == WINDIVERT_EVENT_SOCKET_CONNECT) {
			if (metadata.IPv6 == 0) {
				FiveTuple key{
					.srcAddress = metadata.Socket.LocalAddr[0],
					.srcPort = metadata.Socket.LocalPort,
					.dstAddress = metadata.Socket.RemoteAddr[0],
					.dstPort = metadata.Socket.RemotePort,
					.protocol = metadata.Socket.Protocol
				};

				AssociationEntry entry{
					.processId = metadata.Socket.ProcessId
				};

				m_associations.AddAssociation(key, entry);
				m_guard.AcquireProcess(metadata.Socket.ProcessId);

				FiveTuple searchKey{
					.srcAddress = key.srcAddress,
					.srcPort = key.srcPort,
					.dstAddress = 0,
					.dstPort = 0,
					.protocol = key.protocol
				};

				m_cache->ProcessMatches(searchKey, [&](const Packet& pendingPacket) -> bool {
					auto ctxOpt = WinPacketContext::TryCreate(pendingPacket.data.data(), pendingPacket.length, pendingPacket.metadata);

					if (ctxOpt.has_value()) {
						auto& ctx = ctxOpt.value();
						bool match = true;

						if (metadata.Event == WINDIVERT_EVENT_SOCKET_CONNECT) {
							uint32_t pktDstAddr = WinDivertHelperNtohl(ctx.IsOutbound() ? ctx.GetDestinationAddress() : ctx.GetSourceAddress());
							uint16_t pktDstPort = WinDivertHelperNtohs(ctx.IsOutbound() ? ctx.GetDestinationPort() : ctx.GetSourcePort());
							match = (pktDstAddr == key.dstAddress && pktDstPort == key.dstPort);
						}

						if (match) {
							ctx.SetProcessId(entry.processId);
							callback(ctx);
							return true;
						}
					}

					return false;
					});
			}

			return true;
		}
		else if (metadata.Event == WINDIVERT_EVENT_SOCKET_CLOSE) {
			if (metadata.IPv6 == 0) {
				FiveTuple key{
					.srcAddress = metadata.Socket.LocalAddr[0],
					.srcPort = metadata.Socket.LocalPort,
					.dstAddress = metadata.Socket.RemoteAddr[0],
					.dstPort = metadata.Socket.RemotePort,
					.protocol = metadata.Socket.Protocol
				};

				m_associations.RemoveAssociation(key);
				m_guard.ReleaseProcess(metadata.Socket.ProcessId);
			}

			return true;
		}

		return false;
	}
}