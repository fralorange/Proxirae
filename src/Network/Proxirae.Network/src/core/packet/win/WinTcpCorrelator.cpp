#include <functional>

#include "core/packet/win/WinTcpCorrelator.h"
#include "core/packet/win/WinPacketContext.h"

namespace Proxirae {
	WinTcpCorrelator::WinTcpCorrelator(AssociationTable& associations) 
		: m_associations(associations) { }

	void WinTcpCorrelator::CorrelateNetwork(const Packet& packet, const std::function<void(IPacketContext&)>& callback)
	{
		auto ctxOpt = WinPacketContext::TryCreate(packet.data.data(), packet.length, packet.metadata);

		if (!ctxOpt.has_value()) {
			return;
		}

		auto& ctx = ctxOpt.value();

		bool isOutbound = packet.metadata.Outbound == 1;
		AssociationKey key{
			.srcAddress = WinDivertHelperNtohl(isOutbound ? ctx.GetSourceAddress() : ctx.GetDestinationAddress()),
			.srcPort = WinDivertHelperNtohs(isOutbound ? ctx.GetSourcePort() : ctx.GetDestinationPort()),
			.dstAddress = WinDivertHelperNtohl(isOutbound ? ctx.GetDestinationAddress() : ctx.GetSourceAddress()),
			.dstPort = WinDivertHelperNtohs(isOutbound ? ctx.GetDestinationPort() : ctx.GetSourcePort()),
			.protocol = ctx.GetProtocol()
		};

		if (ctx.IsTcpAck() && ctx.HasPayload() || ctx.IsTcpAck() && !m_associations.AssociationExists(key)) {
			callback(ctx); // Connection existed before we started capturing, so we don't have the socket event for it. Just pass it through.

			return;
		}

		if (m_associations.AssociationExists(key)) {
			auto associationOpt = m_associations.GetAssociation(key);
			ctx.SetProcessId(associationOpt.value().processId);

			callback(ctx);
		} else {
			m_pending.emplace(key, packet);
		}
	}

	void WinTcpCorrelator::CorrelateSocket(const PacketMetadata& metadata, const std::function<void(IPacketContext&)>& callback)
	{
		if (metadata.Event == WINDIVERT_EVENT_SOCKET_CONNECT) {
			if (metadata.IPv6 == 0) {
				AssociationKey key{
					.srcAddress = metadata.Socket.LocalAddr[0],
					.srcPort =	metadata.Socket.LocalPort,
					.dstAddress = metadata.Socket.RemoteAddr[0],
					.dstPort = metadata.Socket.RemotePort,
					.protocol = metadata.Socket.Protocol
				};

				AssociationEntry entry{
					.processId = metadata.Socket.ProcessId
				};	

				m_associations.AddAssociation(key, entry);

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
		}
		else if (metadata.Event == WINDIVERT_EVENT_SOCKET_CLOSE) {
			if (metadata.IPv6 == 0) {
				AssociationKey key{
					.srcAddress = metadata.Socket.LocalAddr[0],
					.srcPort =	metadata.Socket.LocalPort,
					.dstAddress = metadata.Socket.RemoteAddr[0],
					.dstPort = metadata.Socket.RemotePort,
					.protocol = metadata.Socket.Protocol
				};

				m_associations.RemoveAssociation(key);
			}
		}
	}
}