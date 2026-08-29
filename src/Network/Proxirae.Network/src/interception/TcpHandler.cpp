#include <format>

#include "environment/inet.h"
#include "interception/TcpHandler.h"
#include "persistence/ConnectionTable.h"

namespace Proxirae {
	TcpHandler::TcpHandler(std::uint16_t redirectPort, ConnectionTable& connections, ILogger& logger)
		: m_redirectPort(redirectPort), m_connections(connections), m_logger(logger) {}

	bool TcpHandler::CanHandle(const IPacketContext& ctx)
	{
		return ctx.IsTcp();
	}

	void TcpHandler::Handle(PacketDispatchContext& ctx)
	{
		auto& packetCtx = ctx.packetCtx;

		if (packetCtx.IsOutbound()) {
			if (packetCtx.IsTcpRst()) {
				HandleRst(packetCtx);
			}

			if (packetCtx.IsTcpSyn() && !packetCtx.IsTcpAck() && !packetCtx.IsLoopback()) {
				HandleSynOnly(ctx);
			}
			else if (packetCtx.IsTcpSyn() && packetCtx.IsTcpAck() && ntohs(packetCtx.GetSourcePort()) == m_redirectPort) {
				HandleSynAckOnly(packetCtx);
			}
			else if (!packetCtx.IsTcpSyn() && packetCtx.IsTcpAck()) {
				HandleAckOnly(packetCtx);
			}

			if (packetCtx.IsTcpFin()) {
				HandleFin(packetCtx);
			}
		}
	}

	void TcpHandler::HandleSynOnly(PacketDispatchContext& ctx)
	{
		auto& packetCtx = ctx.packetCtx;

		std::string message;

		FiveTuple key{
			.srcAddress = packetCtx.GetSourceAddress(),
			.srcPort = packetCtx.GetSourcePort(),
			.dstAddress = packetCtx.GetDestinationAddress(),
			.dstPort = packetCtx.GetDestinationPort(),
			.protocol = packetCtx.GetProtocol(),
		};

		ConnectionEntry entry{
			.createdAt = GetTickCount64(),
			.lastSeen = GetTickCount64(),
			.proxyId = ctx.proxyId,
			.processId = packetCtx.GetProcessId()
		};

		m_connections.AddConnection(key, entry);

		m_logger.LogDebug(message);

		packetCtx.SetDestination(packetCtx.GetSourceAddress(), htons(m_redirectPort));

		m_logger.LogDebug(std::format(
			"[TcpHandler] Redirected SYN outbound: {} -> {} (redirect port {})",
			packetCtx.GetSourceEndpoint().ToString(),
			packetCtx.GetDestinationEndpoint().ToString(),
			m_redirectPort
		));
	}

	void TcpHandler::HandleSynAckOnly(IPacketContext& ctx)
	{
		ThreeTuple key{
			.srcAddress = ctx.GetDestinationAddress(),
			.srcPort = ctx.GetDestinationPort(),
			.protocol = ctx.GetProtocol(),
		};

		auto it = m_connections.FindKey(key);

		if (it.has_value()) {
			ctx.SetSource(it->dstAddress, it->dstPort);

			m_logger.LogDebug(std::format(
				"[TcpHandler] Restored SYN-ACK source: {} -> {}",
				ctx.GetSourceEndpoint().ToString(),
				ctx.GetDestinationEndpoint().ToString()
			));
		}
	}

	void TcpHandler::HandleAckOnly(IPacketContext& ctx)
	{
		FiveTuple key{
			.srcAddress = ctx.GetSourceAddress(),
			.srcPort = ctx.GetSourcePort(),
			.dstAddress = ctx.GetDestinationAddress(),
			.dstPort = ctx.GetDestinationPort(),
			.protocol = ctx.GetProtocol()
		};

		if (m_connections.ConnectionExists(key)) {
			ctx.SetDestination(ctx.GetSourceAddress(), htons(m_redirectPort));

			m_logger.LogDebug(std::format(
				"[TcpHandler] Redirected ACK outbound: {} -> {} (redirect port {})",
				ctx.GetSourceEndpoint().ToString(),
				ctx.GetDestinationEndpoint().ToString(),
				m_redirectPort
			));
		}
		else {
			ThreeTuple reversalKey{
				.srcAddress = ctx.GetDestinationAddress(),
				.srcPort = ctx.GetDestinationPort(),
				.protocol = ctx.GetProtocol()
			};

			auto it = m_connections.FindKey(reversalKey);

			if (it.has_value()) {
				ctx.SetSource(it->dstAddress, it->dstPort);

				m_logger.LogDebug(std::format(
					"[TcpHandler] Restored ACK source: {} -> {}",
					ctx.GetSourceEndpoint().ToString(),
					ctx.GetDestinationEndpoint().ToString()
				));
			}
		}
	}

	void TcpHandler::HandleRst(IPacketContext& ctx)
	{
		m_logger.LogDebug(std::format(
			"[TcpHandler] RST outbound: {} -> {}",
			ctx.GetSourceEndpoint().ToString(),
			ctx.GetDestinationEndpoint().ToString()
		));
	}

	void TcpHandler::HandleFin(IPacketContext& ctx)
	{ 
		m_logger.LogDebug(std::format(
			"[TcpHandler] FIN outbound: {} -> {}",
			ctx.GetSourceEndpoint().ToString(),
			ctx.GetDestinationEndpoint().ToString()
		));
	}
}