#include <functional>

#include "features/interception/correlation/win/WinTcpCorrelator.h"
#include "features/interception/diversion/win/WinPacketContext.h"
#include "platform/environment/inet.h"
#include <iostream>

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
			.srcAddress = context.IsOutbound() ? context.GetSourceAddress() : context.GetDestinationAddress(),
			.srcPort = context.IsOutbound() ? context.GetSourcePort() : context.GetDestinationPort(),
			.dstAddress = context.IsOutbound() ? context.GetDestinationAddress() : context.GetSourceAddress(),
			.dstPort = context.IsOutbound() ? context.GetDestinationPort() : context.GetSourcePort(),
			.protocol = context.GetProtocol()
		};

		if (context.IsTcpAck() && context.HasPayload() || context.IsTcpAck() && !m_associations.Exists(key)) {
			callback(context); // Connection existed before we started capturing, so we don't have the socket event for it. Just pass it through.

			return true;
		}

		if (m_associations.Exists(key)) {
			auto associationOpt = m_associations.Get(key);
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

		if (metadata.Event == WINDIVERT_EVENT_SOCKET_CONNECT || metadata.Event == WINDIVERT_EVENT_SOCKET_CLOSE)
		{
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

			// Reminder for future: WinDivert always stores SOCKET layer data in Host Byte Order.
			FiveTuple key{
				.srcAddress = srcAddr,
				.srcPort = metadata.Socket.LocalPort,
				.dstAddress = dstAddr,
				.dstPort = metadata.Socket.RemotePort,
				.protocol = metadata.Socket.Protocol
			};

			if (metadata.Event == WINDIVERT_EVENT_SOCKET_CONNECT) {
				AssociationEntry entry{ .processId = metadata.Socket.ProcessId };
				m_associations.Add(key, entry);
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
			else if (metadata.Event == WINDIVERT_EVENT_SOCKET_CLOSE) {
				m_associations.Remove(key);
				m_guard.ReleaseProcess(metadata.Socket.ProcessId);
			}

			return true;
		}

		return false;
	}
}