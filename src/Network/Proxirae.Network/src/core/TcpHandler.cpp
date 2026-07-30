#include <format>
#include <WS2tcpip.h>

#include "core/TcpHandler.h"

constexpr uint16_t DAEMON_PORT = 33999;

namespace Proxirae {
	TcpHandler::TcpHandler(ConnectionTable& connections, ILogger& logger) 
		: m_connections(connections), m_logger(logger) {}

	bool TcpHandler::CanHandle(const Packet& packet)
	{
		return packet.IsTcp();
	}

	void TcpHandler::Handle(Packet& packet)
	{
		if (packet.IsOutbound()) {
			if (packet.IsTcpRst()) {
				HandleRst(packet);
			}

			if (packet.IsTcpSyn() && !packet.IsTcpAck() && !packet.IsLoopback()) {
				HandleSynOnly(packet);
			}
			else if (packet.IsTcpSyn() && packet.IsTcpAck() && ntohs(packet.GetSourcePort()) == DAEMON_PORT) {
				HandleSynAckOnly(packet);
			}
			else if (!packet.IsTcpSyn() && packet.IsTcpAck()) {
				HandleAckOnly(packet);
			}

			if (packet.IsTcpFin()) {
				HandleFin(packet);
			}
		}
	}

	void TcpHandler::HandleSynOnly(Packet& packet)
	{
		std::string message;

		ConnectionKey key{
				.srcAddress = packet.GetSourceAddress(),
				.srcPort = packet.GetSourcePort(),
				.protocol = packet.GetProtocol(),
		};

		ConnectionEntry entry{
			.destAddress = packet.GetDestinationAddress(),
			.destPort = packet.GetDestinationPort(),
			.createdAt = GetTickCount64(),
			.lastSeen = GetTickCount64(),
			.state = ConnectionState::NEW
		};

		m_connections.AddConnection(key, entry);

		message = std::format(
			"Intercepted TCP SYN OUTBOUND packet: Src={} Dst={}",
			packet.GetSourceEndpoint().ToString(),
			packet.GetDestinationEndpoint().ToString()
		);

		m_logger.LogDebug(message);

		packet.SetDestination(packet.GetSourceAddress(), htons(DAEMON_PORT));

		message = std::format(
			"Modified TCP SYN OUTBOUND packet: Dst={}",
			packet.GetDestinationEndpoint().ToString()
		);

		m_logger.LogDebug(message);

		message = std::format(
			"Sent TCP SYN OUTBOUND packet: Src={} Dst={}",
			packet.GetSourceEndpoint().ToString(),
			packet.GetDestinationEndpoint().ToString()
		);

		m_logger.LogDebug(message);
	}

	void TcpHandler::HandleSynAckOnly(Packet& packet)
	{
		std::string message;

		message = std::format(
			"Intercepted TCP SYN ACK OUTBOUND packet: Src={} Dst={}",
			packet.GetSourceEndpoint().ToString(),
			packet.GetDestinationEndpoint().ToString()
		);

		m_logger.LogDebug(message);

		ConnectionKey key{
			.srcAddress = packet.GetDestinationAddress(),
			.srcPort = packet.GetDestinationPort(),
			.protocol = packet.GetProtocol(),
		};

		auto it = m_connections.GetConnection(key);

		if (it.has_value()) {
			packet.SetSource(it->destAddress, it->destPort);

			message = std::format(
				"Modified TCP SYN ACK OUTBOUND packet: Src={}",
				packet.GetSourceEndpoint().ToString()
			);

			m_logger.LogDebug(message);
		}

		message = std::format(
			"Sent TCP SYN ACK OUTBOUND packet: Src={} Dst={}",
			packet.GetSourceEndpoint().ToString(),
			packet.GetDestinationEndpoint().ToString()
		);

		m_logger.LogDebug(message);
	}

	void TcpHandler::HandleAckOnly(Packet& packet)
	{
		std::string message;

		message = std::format(
			"Intercepted TCP ACK OUTBOUND packet: Src={} Dst={}",
			packet.GetSourceEndpoint().ToString(),
			packet.GetDestinationEndpoint().ToString()
		);

		m_logger.LogDebug(message);

		ConnectionKey key{
			.srcAddress = packet.GetSourceAddress(),
			.srcPort = packet.GetSourcePort(),
			.protocol = packet.GetProtocol()
		};

		if (m_connections.ConnectionExists(key)) {
			packet.SetDestination(packet.GetSourceAddress(), htons(DAEMON_PORT));

			message = std::format(
				"Modified TCP ACK OUTBOUND packet: Dst={}",
				packet.GetDestinationEndpoint().ToString()
			);

			m_logger.LogDebug(message);
		}

		ConnectionKey reversalKey{
			.srcAddress = packet.GetDestinationAddress(),
			.srcPort = packet.GetDestinationPort(),
			.protocol = packet.GetProtocol()
		};

		auto it = m_connections.GetConnection(reversalKey);

		if (it.has_value()) {
			packet.SetSource(it->destAddress, it->destPort);

			message = std::format(
				"Modified TCP ACK OUTBOUND packet: Src={}",
				packet.GetSourceEndpoint().ToString()
			);

			m_logger.LogDebug(message);
		}

		message = std::format(
			"Sent TCP ACK OUTBOUND packet: Src={} Dst={}",
			packet.GetSourceEndpoint().ToString(),
			packet.GetDestinationEndpoint().ToString()
		);

		m_logger.LogDebug(message);
	}

	void TcpHandler::HandleRst(Packet& packet)
	{
		std::string message;

		message = std::format(
			"Intercepted TCP RST OUTBOUND packet: Src={} Dst={}",
			packet.GetSourceEndpoint().ToString(),
			packet.GetDestinationEndpoint().ToString()
		);

		m_logger.LogDebug(message);

		ConnectionKey key{
			.srcAddress = packet.GetSourceAddress(),
			.srcPort = packet.GetSourcePort(),
			.protocol = packet.GetProtocol()
		};

		if (m_connections.ConnectionExists(key)) {
			m_connections.RemoveConnection(key);
		}
	}

	void TcpHandler::HandleFin(Packet& packet)
	{ 
		std::string message;

		message = std::format(
			"Intercepted TCP FIN OUTBOUND packet: Src={} Dst={}",
			packet.GetSourceEndpoint().ToString(),
			packet.GetDestinationEndpoint().ToString()
		);

		m_logger.LogDebug(message);

		ConnectionKey key{
			.srcAddress = packet.GetSourceAddress(),
			.srcPort = packet.GetSourcePort(),
			.protocol = packet.GetProtocol()
		};

		if (m_connections.GetState(key) != ConnectionState::CLOSED) {
			m_connections.SetState(key, ConnectionState::CLOSED);
		}
	}
}