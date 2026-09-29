#include "interception/correlation/win/WinUdpCorrelator.h"
#include "interception/diversion/win/WinPacketContext.h"
#include "environment/inet.h"

namespace Proxirae {
	WinUdpCorrelator::WinUdpCorrelator(IProcessGuard& monitor, AssociationTable& associations)
		: m_guard(monitor), m_associations(associations), m_lookupTable(WinUdpLookupTable::TryCreate()) {
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

		if (m_lookupTable.has_value() && (m_lookupTable->CanAssociate(bindKey) || m_lookupTable->CanAssociate(anyBindKey))) {
			callback(context); // Connection existed before we started capturing, so we don't have the socket event for it. Just pass it through.

			return true;
		}

		if (m_associations.AssociationExists(fullKey)) {
			context.SetProcessId(m_associations.GetAssociation(fullKey).value().processId);
			callback(context);
		}
		else if (m_associations.AssociationExists(bindKey)) {
			context.SetProcessId(m_associations.GetAssociation(bindKey).value().processId);
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

			m_associations.AddAssociation(key, entry);
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
			if (m_associations.AssociationExists(key)) {
				m_associations.RemoveAssociation(key);
				m_guard.ReleaseProcess(metadata.Socket.ProcessId);
			}

			FiveTuple bindKey = key;
			bindKey.dstAddress = IpAddress{};
			bindKey.dstAddress.isIPv6 = key.dstAddress.isIPv6;
			bindKey.dstPort = 0;

			if (m_associations.AssociationExists(bindKey)) {
				m_associations.RemoveAssociation(bindKey);
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