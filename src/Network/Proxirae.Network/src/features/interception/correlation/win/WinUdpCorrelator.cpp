#include "features/interception/correlation/win/WinUdpCorrelator.h"
#include "features/interception/diversion/win/WinPacketContext.h"
#include "platform/environment/inet.h"

namespace Proxirae {
	WinUdpCorrelator::WinUdpCorrelator(IProcessGuard& monitor, AssociationTable& associations, std::optional<WinUdpLookupTable> lookupTable, std::uint16_t redirectPort)
		: m_guard(monitor), m_associations(associations), m_lookupTable(std::move(lookupTable)), m_redirectPort(redirectPort) {
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
			.srcAddress = context.IsOutbound() ? context.GetSourceAddress() : context.GetDestinationAddress(),
			.srcPort = context.IsOutbound() ? context.GetSourcePort() : context.GetDestinationPort(),
			.dstAddress = context.IsOutbound() ? context.GetDestinationAddress() : context.GetSourceAddress(),
			.dstPort = context.IsOutbound() ? context.GetDestinationPort() : context.GetSourcePort(),
			.protocol = context.GetProtocol()
		};

		FiveTuple bindKey = fullKey;
		bindKey.dstAddress = IpAddress{};
		bindKey.dstAddress.isIPv6 = fullKey.dstAddress.isIPv6;
		bindKey.dstPort = 0;

		FiveTuple anyBindKey = bindKey;
		anyBindKey.srcAddress = IpAddress{};
		anyBindKey.srcAddress.isIPv6 = fullKey.srcAddress.isIPv6;

		const bool isPreExistingConnection =
			m_lookupTable.has_value() &&
			(m_lookupTable->CanAssociate(bindKey) ||
				m_lookupTable->CanAssociate(anyBindKey)); // Connection existed before we started capturing, so we don't have the socket event for it. Just pass it through.

		const bool isRedirectTraffic =
			context.IsOutbound() &&
			context.GetSourcePort() == m_redirectPort;

		if (isPreExistingConnection || isRedirectTraffic) {
			callback(context); 

			return true;
		}

		if (m_associations.Exists(fullKey)) {
			context.SetProcessId(m_associations.Get(fullKey).value().processId);
			callback(context);
		}
		else if (m_associations.Exists(bindKey)) {
			context.SetProcessId(m_associations.Get(bindKey).value().processId);
			callback(context);
		}
		else {
			auto* rawData = context.GetRawData();
			auto dataLen = context.GetRawDataLength();

			m_pending.emplace(fullKey, Packet{
				std::vector<std::uint8_t>(rawData, rawData + dataLen),
				dataLen,
				context.GetMetadata()
			});
		}

		return true;
	}

	bool WinUdpCorrelator::CorrelateSocket(const PacketMetadata& metadata, const std::function<void(IPacketContext&)>& callback)
	{
		if (metadata.Socket.Protocol != IPPROTO_UDP) {
			return false;
		}

		bool isIPv6 = (metadata.IPv6 == 1);

		IpAddress srcAddr{};
		srcAddr.isIPv6 = isIPv6;
		if (isIPv6) {
			std::copy(std::begin(metadata.Socket.LocalAddr), std::end(metadata.Socket.LocalAddr), srcAddr.data.begin());
		}
		else {
			srcAddr.data[0] = metadata.Socket.LocalAddr[0];
		}

		IpAddress dstAddr{};
		dstAddr.isIPv6 = isIPv6;
		if (isIPv6) {
			std::copy(std::begin(metadata.Socket.RemoteAddr), std::end(metadata.Socket.RemoteAddr), dstAddr.data.begin());
		}
		else {
			dstAddr.data[0] = metadata.Socket.RemoteAddr[0];
		}

		FiveTuple key{
			.srcAddress = srcAddr,
			.srcPort = metadata.Socket.LocalPort,
			.dstAddress = dstAddr,
			.dstPort = metadata.Socket.RemotePort,
			.protocol = metadata.Socket.Protocol
		};

		if (metadata.Event == WINDIVERT_EVENT_SOCKET_BIND || metadata.Event == WINDIVERT_EVENT_SOCKET_CONNECT) {
			AssociationEntry entry{
				.processId = metadata.Socket.ProcessId
			};

			m_associations.Add(key, entry);
			m_guard.AcquireProcess(metadata.Socket.ProcessId);

			for (auto it = m_pending.begin(); it != m_pending.end(); ) {
				const auto& [pktKey, pendingPacket] = *it;

				bool srcMatch = (pktKey.srcAddress == key.srcAddress) || key.srcAddress.IsAny();
				bool match = srcMatch && (pktKey.srcPort == key.srcPort) && (pktKey.protocol == key.protocol);

				if (match && metadata.Event == WINDIVERT_EVENT_SOCKET_CONNECT) {
					match = (pktKey.dstAddress == key.dstAddress && pktKey.dstPort == key.dstPort);
				}

				if (match) {
					auto ctxOpt = WinPacketContext::TryCreate(
						pendingPacket.data.data(),
						pendingPacket.length,
						pendingPacket.metadata
					);

					if (ctxOpt.has_value()) {
						auto& ctx = ctxOpt.value();
						ctx.SetProcessId(entry.processId);
						callback(ctx);
					}

					it = m_pending.erase(it);
				}
				else {
					++it;
				}
			}

			return true;
		}
		else if (metadata.Event == WINDIVERT_EVENT_SOCKET_CLOSE) {
			if (m_associations.Exists(key)) {
				m_associations.Remove(key);
				m_guard.ReleaseProcess(metadata.Socket.ProcessId);
			}

			FiveTuple bindKey = key;
			bindKey.dstAddress = IpAddress{};
			bindKey.dstAddress.isIPv6 = key.dstAddress.isIPv6;
			bindKey.dstPort = 0;

			if (m_associations.Exists(bindKey)) {
				m_associations.Remove(bindKey);
				m_guard.ReleaseProcess(metadata.Socket.ProcessId);
			}

			if (m_lookupTable.has_value()) {
				m_lookupTable->TryRemoveBind(bindKey);

				if (m_lookupTable->IsEmpty()) {
					m_lookupTable.reset();
				}
			}

			return true;
		}

		return false;
	}
}