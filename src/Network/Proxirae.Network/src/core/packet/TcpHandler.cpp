#include <format>
#include <WS2tcpip.h>

#include "core/packet/TcpHandler.h"
#include <core/registry/ConnectionTable.h>

namespace Proxirae {
	TcpHandler::TcpHandler(std::uint16_t redirectPort, ConnectionTable& connections, ILogger& logger)
		: m_redirectPort(redirectPort), m_connections(connections), m_logger(logger) {}

	bool TcpHandler::CanHandle(const IPacketContext& packet)
	{
		return packet.IsTcp();
	}

	void TcpHandler::Handle(IPacketContext& ctx)
	{
		if (ctx.IsOutbound()) {
			if (ctx.IsTcpRst()) {
				HandleRst(ctx);
			}

			if (ctx.IsTcpSyn() && !ctx.IsTcpAck() && !ctx.IsLoopback()) {
				HandleSynOnly(ctx);
			}
			else if (ctx.IsTcpSyn() && ctx.IsTcpAck() && ntohs(ctx.GetSourcePort()) == m_redirectPort) {
				HandleSynAckOnly(ctx);
			}
			else if (!ctx.IsTcpSyn() && ctx.IsTcpAck()) {
				HandleAckOnly(ctx);
			}

			if (ctx.IsTcpFin()) {
				HandleFin(ctx);
			}
		}
	}

	void TcpHandler::HandleSynOnly(IPacketContext& ctx)
	{
		std::string message;

		ConnectionKey key{
				.srcAddress = ctx.GetSourceAddress(),
				.srcPort = ctx.GetSourcePort(),
				.protocol = ctx.GetProtocol(),
		};

		ConnectionEntry entry{
			.destAddress = ctx.GetDestinationAddress(),
			.destPort = ctx.GetDestinationPort(),
			.createdAt = GetTickCount64(),
			.lastSeen = GetTickCount64(),
			.state = ConnectionState::NEW
		};

		m_connections.AddConnection(key, entry);

		message = std::format(
			"Intercepted TCP SYN OUTBOUND packet: Src={} Dst={}",
			ctx.GetSourceEndpoint().ToString(),
			ctx.GetDestinationEndpoint().ToString()
		);

		m_logger.LogDebug(message);

		ctx.SetDestination(ctx.GetSourceAddress(), htons(m_redirectPort));

		message = std::format(
			"Modified TCP SYN OUTBOUND packet: Dst={}",
			ctx.GetDestinationEndpoint().ToString()
		);

		m_logger.LogDebug(message);

		message = std::format(
			"Sent TCP SYN OUTBOUND packet: Src={} Dst={}",
			ctx.GetSourceEndpoint().ToString(),
			ctx.GetDestinationEndpoint().ToString()
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

		ConnectionKey key{
			.srcAddress = ctx.GetDestinationAddress(),
			.srcPort = ctx.GetDestinationPort(),
			.protocol = ctx.GetProtocol(),
		};

		auto it = m_connections.GetConnection(key);

		if (it.has_value()) {
			ctx.SetSource(it->destAddress, it->destPort);

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

		ConnectionKey key{
			.srcAddress = ctx.GetSourceAddress(),
			.srcPort = ctx.GetSourcePort(),
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

		ConnectionKey reversalKey{
			.srcAddress = ctx.GetDestinationAddress(),
			.srcPort = ctx.GetDestinationPort(),
			.protocol = ctx.GetProtocol()
		};

		auto it = m_connections.GetConnection(reversalKey);

		if (it.has_value()) {
			ctx.SetSource(it->destAddress, it->destPort);

			message = std::format(
				"Modified TCP ACK OUTBOUND packet: Src={}",
				ctx.GetSourceEndpoint().ToString()
			);

			m_logger.LogDebug(message);
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

		ConnectionKey key{
			.srcAddress = ctx.GetSourceAddress(),
			.srcPort = ctx.GetSourcePort(),
			.protocol = ctx.GetProtocol()
		};

		if (m_connections.ConnectionExists(key)) {
			m_connections.RemoveConnection(key);
		}
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

		ConnectionKey key{
			.srcAddress = ctx.GetSourceAddress(),
			.srcPort = ctx.GetSourcePort(),
			.protocol = ctx.GetProtocol()
		};

		if (m_connections.GetState(key) != ConnectionState::CLOSED) {
			m_connections.SetState(key, ConnectionState::CLOSED);
		}
	}
}