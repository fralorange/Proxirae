#include <format>

#include "platform/inet.h"
#include "packet/TcpHandler.h"
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
		};

		m_connections.AddConnection(key, entry);

		message = std::format(
			"Intercepted TCP SYN OUTBOUND packet: Src={} Dst={}",
			packetCtx.GetSourceEndpoint().ToString(),
			packetCtx.GetDestinationEndpoint().ToString()
		);

		m_logger.LogDebug(message);

		packetCtx.SetDestination(packetCtx.GetSourceAddress(), htons(m_redirectPort));

		message = std::format(
			"Modified TCP SYN OUTBOUND packet: Dst={}",
			packetCtx.GetDestinationEndpoint().ToString()
		);

		m_logger.LogDebug(message);

		message = std::format(
			"Sent TCP SYN OUTBOUND packet: Src={} Dst={}",
			packetCtx.GetSourceEndpoint().ToString(),
			packetCtx.GetDestinationEndpoint().ToString()
		);

		m_logger.LogDebug(message);
	}

	void TcpHandler::HandleSynAckOnly(IPacketContext& ctx)
	{
		std::string message;

		message = std::format(
			"Intercepted TCP SYN ACK OUTBOUND packet: Src={} Dst={}",
			ctx.GetSourceEndpoint().ToString(),
			ctx.GetDestinationEndpoint().ToString()
		);

		m_logger.LogDebug(message);

		ThreeTuple key{
			.srcAddress = ctx.GetDestinationAddress(),
			.srcPort = ctx.GetDestinationPort(),
			.protocol = ctx.GetProtocol(),
		};

		auto it = m_connections.FindKey(key);

		if (it.has_value()) {
			ctx.SetSource(it->dstAddress, it->dstPort);

			message = std::format(
				"Modified TCP SYN ACK OUTBOUND packet: Src={}",
				ctx.GetSourceEndpoint().ToString()
			);

			m_logger.LogDebug(message);
		}

		message = std::format(
			"Sent TCP SYN ACK OUTBOUND packet: Src={} Dst={}",
			ctx.GetSourceEndpoint().ToString(),
			ctx.GetDestinationEndpoint().ToString()
		);

		m_logger.LogDebug(message);
	}

	void TcpHandler::HandleAckOnly(IPacketContext& ctx)
	{
		std::string message;

		message = std::format(
			"Intercepted TCP ACK OUTBOUND packet: Src={} Dst={}",
			ctx.GetSourceEndpoint().ToString(),
			ctx.GetDestinationEndpoint().ToString()
		);

		m_logger.LogDebug(message);

		FiveTuple key{
			.srcAddress = ctx.GetSourceAddress(),
			.srcPort = ctx.GetSourcePort(),
			.dstAddress = ctx.GetDestinationAddress(),
			.dstPort = ctx.GetDestinationPort(),
			.protocol = ctx.GetProtocol()
		};

		if (m_connections.ConnectionExists(key)) {
			ctx.SetDestination(ctx.GetSourceAddress(), htons(m_redirectPort));

			message = std::format(
				"Modified TCP ACK OUTBOUND packet: Dst={}",
				ctx.GetDestinationEndpoint().ToString()
			);

			m_logger.LogDebug(message);
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

				message = std::format(
					"Modified TCP ACK OUTBOUND packet: Src={}",
					ctx.GetSourceEndpoint().ToString()
				);

				m_logger.LogDebug(message);
			}
		}

		message = std::format(
			"Sent TCP ACK OUTBOUND packet: Src={} Dst={}",
			ctx.GetSourceEndpoint().ToString(),
			ctx.GetDestinationEndpoint().ToString()
		);

		m_logger.LogDebug(message);
	}

	void TcpHandler::HandleRst(IPacketContext& ctx)
	{
		std::string message;

		message = std::format(
			"Intercepted TCP RST OUTBOUND packet: Src={} Dst={}",
			ctx.GetSourceEndpoint().ToString(),
			ctx.GetDestinationEndpoint().ToString()
		);

		m_logger.LogDebug(message);
	}

	void TcpHandler::HandleFin(IPacketContext& ctx)
	{ 
		std::string message;

		message = std::format(
			"Intercepted TCP FIN OUTBOUND packet: Src={} Dst={}",
			ctx.GetSourceEndpoint().ToString(),
			ctx.GetDestinationEndpoint().ToString()
		);

		m_logger.LogDebug(message);
	}
}