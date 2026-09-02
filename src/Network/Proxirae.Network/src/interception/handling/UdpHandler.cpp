#include <format>

#include "environment/inet.h"
#include "interception/handling/UdpHandler.h"

namespace Proxirae {
	UdpHandler::UdpHandler(std::uint16_t redirectPort, ConnectionTable& connections, ILogger& logger)
		: m_redirectPort(redirectPort), m_connections(connections), m_logger(logger) { }

	bool UdpHandler::Handle(HandleContext& ctx)
	{
		if (!ctx.packetCtx.IsUdp()) {
			return false;
		}

		auto& packetCtx = ctx.packetCtx;

		if (packetCtx.IsOutbound()) {
			if (WinDivertHelperNtohs(packetCtx.GetSourcePort()) == m_redirectPort) {
				ThreeTuple key{
					.srcAddress = packetCtx.GetDestinationAddress(),
					.srcPort = packetCtx.GetDestinationPort(),
					.protocol = packetCtx.GetProtocol(),
				};

				auto it = m_connections.FindKey(key);

				if (it.has_value()) {
					packetCtx.SetSource(it->dstAddress, it->dstPort);

					m_logger.LogDebug(std::format(
						"[UdpHandler] Restored UDP source: {} -> {}",
						packetCtx.GetSourceEndpoint().ToString(),
						packetCtx.GetDestinationEndpoint().ToString()
					));

					m_connections.KeepAlive(key, GetTickCount64());
				}
			}
			else {
				FiveTuple key{
					.srcAddress = packetCtx.GetSourceAddress(),
					.srcPort = packetCtx.GetSourcePort(),
					.dstAddress = packetCtx.GetDestinationAddress(),
					.dstPort = packetCtx.GetDestinationPort(),
					.protocol = packetCtx.GetProtocol()
				};

				if (!m_connections.ConnectionExists(key)) {
					ConnectionEntry entry{
						.createdAt = GetTickCount64(),
						.lastSeen = GetTickCount64(),
						.proxyId = ctx.proxyId,
						.processId = packetCtx.GetProcessId()
					};

					m_connections.AddConnection(key, entry);

					m_logger.LogDebug(std::format(
						"[UdpHandler] Recorded new UDP session: {} -> {}",
						packetCtx.GetSourceEndpoint().ToString(),
						packetCtx.GetDestinationEndpoint().ToString()
					));
				}
				else {
					m_connections.KeepAlive(key, GetTickCount64());
				}

				packetCtx.SetDestination(packetCtx.GetSourceAddress(), WinDivertHelperHtons(m_redirectPort));

				m_logger.LogDebug(std::format(
					"[UdpHandler] Redirected UDP outbound: {} -> {} (redirect port {})",
					packetCtx.GetSourceEndpoint().ToString(),
					packetCtx.GetDestinationEndpoint().ToString(),
					m_redirectPort
				));
			}
		}

		return true;
	}
}