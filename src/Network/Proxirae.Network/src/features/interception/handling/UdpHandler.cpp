#include <format>

#include "features/interception/handling/UdpHandler.h"

namespace Proxirae {
	UdpHandler::UdpHandler(std::uint16_t redirectPort, ConnectionTable& connections, VirtualTable& virtuals, ILogger& logger)
		: m_redirectPort(redirectPort), m_connections(connections), m_virtuals(virtuals), m_logger(logger) { }

	bool UdpHandler::Handle(HandleContext& ctx)
	{
		if (!ctx.packetCtx.IsUdp()) {
			return false;
		}

		auto& packetCtx = ctx.packetCtx;

		if (packetCtx.IsOutbound()) {
			if (packetCtx.GetSourcePort() == m_redirectPort) {
				ThreeTuple vKey{
					.srcAddress = packetCtx.GetDestinationAddress(),
					.srcPort = packetCtx.GetDestinationPort(),
					.protocol = packetCtx.GetProtocol(),
				};

				auto optVirtual = m_virtuals.Resolve(vKey);

				if (optVirtual.has_value()) {
					const auto& real = optVirtual->realTuple;

					packetCtx.SetSource(real.dstAddress, real.dstPort);
					packetCtx.SetDestination(real.srcAddress, real.srcPort);

					m_logger.LogDebug(std::format(
						"[UdpHandler] Restored UDP source: {} -> {}",
						packetCtx.GetSourceEndpoint().ToString(),
						packetCtx.GetDestinationEndpoint().ToString()
					));
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

				if (!m_connections.Exists(key)) {
					ConnectionEntry entry{
						.proxyId = ctx.proxyId,
						.processId = packetCtx.GetProcessId()
					};

					m_connections.Add(key, entry);

					m_logger.LogDebug(std::format(
						"[UdpHandler] Recorded new UDP session: {} -> {}",
						packetCtx.GetSourceEndpoint().ToString(),
						packetCtx.GetDestinationEndpoint().ToString()
					));
				}

				auto vPort = m_virtuals.FindOrAdd({ key });
				if (vPort == 0) {
					m_logger.LogError("[UdpHandler] VirtualTable port pool exhausted!");
					return false;
				}

				m_connections.AddAlias({ key.srcAddress, vPort, key.protocol }, key);

				packetCtx.SetSource(packetCtx.GetSourceAddress(), vPort);
				packetCtx.SetDestination(packetCtx.GetSourceAddress(), m_redirectPort);

				m_logger.LogDebug(std::format(
					"[UdpHandler] Redirected UDP outbound: {} (vPort {}) -> redirect port {}",
					key.srcAddress.ToString(),
					vPort,
					m_redirectPort
				));
			}
		}

		return true;
	}
}
