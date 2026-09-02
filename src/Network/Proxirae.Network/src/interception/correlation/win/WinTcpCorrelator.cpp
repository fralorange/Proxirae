#include <functional>

#include "environment/inet.h"
#include "interception/correlation/win/WinTcpCorrelator.h"
#include "interception/diversion/win/WinPacketContext.h"

namespace Proxirae {
	WinTcpCorrelator::WinTcpCorrelator(IProcessGuard& monitor, AssociationTable& associations)
		: m_guard(monitor), m_associations(associations) {
	}

	bool WinTcpCorrelator::CorrelateNetwork(const Packet& packet, const std::function<void(IPacketContext&)>& callback)
	{
		auto ctxOpt = WinPacketContext::TryCreate(packet.data.data(), packet.length, packet.metadata);

		if (!ctxOpt.has_value() || !ctxOpt->IsTcp()) {
			return false;
		}

		auto& context = ctxOpt.value();

		FiveTuple key{
			.srcAddress = WinDivertHelperNtohl(context.IsOutbound() ? context.GetSourceAddress() : context.GetDestinationAddress()),
			.srcPort = WinDivertHelperNtohs(context.IsOutbound() ? context.GetSourcePort() : context.GetDestinationPort()),
			.dstAddress = WinDivertHelperNtohl(context.IsOutbound() ? context.GetDestinationAddress() : context.GetSourceAddress()),
			.dstPort = WinDivertHelperNtohs(context.IsOutbound() ? context.GetDestinationPort() : context.GetSourcePort()),
			.protocol = context.GetProtocol()
		};

		if (context.IsTcpAck() && context.HasPayload() || context.IsTcpAck() && !m_associations.AssociationExists(key)) {
			callback(context); // Connection existed before we started capturing, so we don't have the socket event for it. Just pass it through.

			return true;
		}

		if (m_associations.AssociationExists(key)) {
			auto associationOpt = m_associations.GetAssociation(key);
			context.SetProcessId(associationOpt.value().processId);

			callback(context);
		}
		else {
			auto* rawData = context.GetRawData();
			auto dataLen = context.GetRawDataLength();

			m_pending.emplace(key, Packet{
				std::vector<std::uint8_t>(rawData, rawData + dataLen),
				dataLen,
				context.GetMetadata()
				});
		}

		return true;
	}

	bool WinTcpCorrelator::CorrelateSocket(const PacketMetadata& metadata, const std::function<void(IPacketContext&)>& callback)
	{
		if (metadata.Socket.Protocol != IPPROTO_TCP) {
			return false;
		}

		if (metadata.Event == WINDIVERT_EVENT_SOCKET_CONNECT) {
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

				auto [it, end] = m_pending.equal_range(key);

				while (it != end) {
					auto& pendingPacket = it->second;

					auto ctxOpt = WinPacketContext::TryCreate(pendingPacket.data.data(), pendingPacket.length, pendingPacket.metadata);

					if (ctxOpt.has_value()) {
						auto& ctx = ctxOpt.value();
						ctx.SetProcessId(entry.processId);

						callback(ctx);
					}

					it = m_pending.erase(it);
				}
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