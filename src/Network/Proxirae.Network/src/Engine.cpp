#include <string>
#include <format>
#include <ws2tcpip.h>

#include "Engine.h"
#include "Packet.h"

#pragma comment(lib, "Ws2_32.lib")

constexpr uint16_t ALT_PORT = 33999;

namespace Proxirae::Network {
	Engine::Engine(Persistence::ConnectionTable& connections, Logging::ILogger& logger) 
		:	m_packetBuffer(0xFFFF), 
			m_connections(connections), 
			m_logger(logger),
			m_handle(INVALID_HANDLE_VALUE),
			m_recvLen(0),
			m_addr(),
			m_running(false) {}

	Engine::~Engine() {
		Stop();
	}

	void Engine::Start() {
		const char* filter = "ip and tcp and tcp.SrcPort != 25344 and tcp.DstPort != 25344 and tcp.SrcPort != 10808 and tcp.DstPort != 10808";

		m_handle = WinDivertOpen(filter, WINDIVERT_LAYER_NETWORK, 123, 0);

		if (m_handle == INVALID_HANDLE_VALUE)
		{
			std::string message = std::format("Failed to open WinDivert handle: {}", GetLastError());
			m_logger.LogCritical(message);

			return;
		}

		m_running = true;

		while (m_running) {
			if (!WinDivertRecv(m_handle, m_packetBuffer.data(), m_packetBuffer.size(), &m_recvLen, &m_addr)) {
				std::string message = std::format("Failed to receive packet: {}", GetLastError());
				m_logger.LogError(message);
				continue;
			}

			Packet packet;

			if (!WinDivertHelperParsePacket(
				m_packetBuffer.data(), m_recvLen, &packet.ipHdr, &packet.ipv6Hdr, &packet.protocol, nullptr, nullptr, &packet.tcpHdr, &packet.udpHdr, nullptr, nullptr, nullptr, nullptr
			)) {
				m_logger.LogError("Failed to parse packet.");
				continue;
			}

			if (m_addr.Outbound) {
				if (packet.ipHdr && packet.tcpHdr) {
					if (packet.tcpHdr->Syn && !packet.tcpHdr->Ack && !m_addr.Loopback) {
						Persistence::ConnectionKey key{
							.srcAddress = packet.ipHdr->SrcAddr,
							.srcPort = packet.tcpHdr->SrcPort,
							.protocol = packet.protocol
						};

						Persistence::ConnectionEntry entry{
							.destAddress = packet.ipHdr->DstAddr,
							.destPort = packet.tcpHdr->DstPort,
							.createdAt = GetTickCount64(),
							.lastSeen = GetTickCount64(),
							.state = Persistence::ConnectionState::NEW
						};

						m_connections.AddConnection(key, entry);

						char dstIpStr[INET_ADDRSTRLEN];

						inet_ntop(AF_INET, &packet.ipHdr->DstAddr, dstIpStr, sizeof(dstIpStr));
						int dstPortStr = ntohs(packet.tcpHdr->DstPort);

						std::string message = std::format(
							"Changing TCP SYN packet destination from {}:{} to ",
							dstIpStr,
							dstPortStr
						);

						packet.ipHdr->DstAddr = packet.ipHdr->SrcAddr;
						packet.tcpHdr->DstPort = htons(ALT_PORT);

						inet_ntop(AF_INET, &packet.ipHdr->DstAddr, dstIpStr, sizeof(dstIpStr));
						dstPortStr = ntohs(packet.tcpHdr->DstPort);

						message += std::format("{}:{}", dstIpStr, dstPortStr);

						m_logger.LogDebug(message);

						m_addr.Outbound = 0;

						char srcIpStr[INET_ADDRSTRLEN];

						inet_ntop(AF_INET, &packet.ipHdr->SrcAddr, srcIpStr, sizeof(srcIpStr));
						inet_ntop(AF_INET, &packet.ipHdr->DstAddr, dstIpStr, sizeof(dstIpStr));

						int srcPortStr = ntohs(packet.tcpHdr->SrcPort);
						dstPortStr = ntohs(packet.tcpHdr->DstPort);

						message = std::format(
							"Sending TCP SYN packet: {}:{} to {}:{}",
							srcIpStr,
							srcPortStr,
							dstIpStr,
							dstPortStr
						);

						m_logger.LogDebug(message);
					} else if (packet.tcpHdr->Syn && packet.tcpHdr->Ack && ntohs(packet.tcpHdr->SrcPort) == ALT_PORT) {
						Persistence::ConnectionKey key{
							.srcAddress = packet.ipHdr->DstAddr,
							.srcPort = packet.tcpHdr->DstPort,
							.protocol = packet.protocol,
						};

						auto it = m_connections.GetConnection(key);

						if (it.has_value()) {
							packet.ipHdr->SrcAddr = it->destAddress;
							packet.tcpHdr->SrcPort = it->destPort;
						}

						char srcIpStr[INET_ADDRSTRLEN];
						char dstIpStr[INET_ADDRSTRLEN];

						inet_ntop(AF_INET, &packet.ipHdr->SrcAddr, srcIpStr, sizeof(srcIpStr));
						inet_ntop(AF_INET, &packet.ipHdr->DstAddr, dstIpStr, sizeof(dstIpStr));

						int srcPortStr = ntohs(packet.tcpHdr->SrcPort);
						int dstPortStr = ntohs(packet.tcpHdr->DstPort);

						std::string message = std::format(
							"Sending TCP SYN ACK packet: {}:{} to {}:{}",
							srcIpStr,
							srcPortStr,
							dstIpStr,
							dstPortStr
						);

						m_logger.LogDebug(message);
					}
					else if (!packet.tcpHdr->Syn && packet.tcpHdr->Ack && !m_addr.Loopback) {
						packet.ipHdr->DstAddr = packet.ipHdr->SrcAddr;
						packet.tcpHdr->DstPort = htons(ALT_PORT);

						char srcIpStr[INET_ADDRSTRLEN];
						char dstIpStr[INET_ADDRSTRLEN];

						inet_ntop(AF_INET, &packet.ipHdr->SrcAddr, srcIpStr, sizeof(srcIpStr));
						inet_ntop(AF_INET, &packet.ipHdr->DstAddr, dstIpStr, sizeof(dstIpStr));

						int srcPortStr = ntohs(packet.tcpHdr->SrcPort);
						int dstPortStr = ntohs(packet.tcpHdr->DstPort);

						std::string message = std::format(
							"Sending TCP ACK packet: {}:{} to {}:{}",
							srcIpStr,
							srcPortStr,
							dstIpStr,
							dstPortStr
						);

						m_logger.LogDebug(message);
					}
				}
			}
			else {
				if (packet.ipHdr && packet.tcpHdr) {

;				}
			}

			if (!WinDivertHelperCalcChecksums(m_packetBuffer.data(), m_recvLen, &m_addr, 0)) {
				m_logger.LogError("Failed to calculate checksums.");
				continue;
			}

			if (!WinDivertSend(m_handle, m_packetBuffer.data(), m_recvLen, nullptr, &m_addr)) {
				std::string message = std::format("Failed to send packet: {}", GetLastError());
				m_logger.LogError(message);
				continue;
			}
		}
	}

	void Engine::Stop() {
		if (!m_running)
			return;

		m_running = false;

		if (m_handle != INVALID_HANDLE_VALUE)
		{
			WinDivertShutdown(m_handle, WINDIVERT_SHUTDOWN_BOTH);
			WinDivertClose(m_handle);
			m_handle = INVALID_HANDLE_VALUE;
		}
	}
}
