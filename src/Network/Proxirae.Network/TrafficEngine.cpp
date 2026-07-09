#include <WinSock2.h>
#include <ws2tcpip.h>
#include <iostream>

#include "TrafficEngine.h"
#include "PacketInfo.h"

#pragma comment(lib, "Ws2_32.lib")

namespace Proxirae {
	namespace Network {
		TrafficEngine::TrafficEngine() : m_packetBuffer(0xFFFF) {
			const char* filter = "(ip or ipv6) and (tcp or udp)";

			m_handle = WinDivertOpen(filter, WINDIVERT_LAYER_NETWORK, 0, 0);

			if (m_handle == INVALID_HANDLE_VALUE)
			{
				throw std::runtime_error(
					"Failed to open WinDivert handle"
				);
			}

			m_recvLen = 0;
			m_addr = {};
			m_running = false;
		}

		TrafficEngine::~TrafficEngine() {
			Stop();

			if (m_handle != INVALID_HANDLE_VALUE)
			{
				WinDivertClose(m_handle);
			}
		}

		void TrafficEngine::Start() {
			m_running = true;

			while (m_running) {
				if (!WinDivertRecv(m_handle, m_packetBuffer.data(), m_packetBuffer.size(), &m_recvLen, &m_addr)) {
					std::cerr << "Failed to receive packet." << std::endl;
					continue;
				}

				PacketInfo packetInfo;

				char srcAddrStr[INET6_ADDRSTRLEN];
				char dstAddrStr[INET6_ADDRSTRLEN];

				unsigned short srcPort;
				unsigned short dstPort;

				if (!WinDivertHelperParsePacket(
					m_packetBuffer.data(), m_recvLen, &packetInfo.ipHdr, &packetInfo.ipv6Hdr, &packetInfo.protocol, nullptr, nullptr, &packetInfo.tcpHdr, &packetInfo.udpHdr, nullptr, nullptr, nullptr, nullptr
				)) {
					std::cerr << "Failed to parse packet." << std::endl;
				}
				else {
					if (packetInfo.ipHdr) {
						inet_ntop(AF_INET, &packetInfo.ipHdr->SrcAddr, srcAddrStr, sizeof(srcAddrStr));
						inet_ntop(AF_INET, &packetInfo.ipHdr->DstAddr, dstAddrStr, sizeof(dstAddrStr));
					}
					else if (packetInfo.ipv6Hdr) {
						inet_ntop(AF_INET, &packetInfo.ipv6Hdr->SrcAddr, srcAddrStr, sizeof(srcAddrStr));
						inet_ntop(AF_INET, &packetInfo.ipv6Hdr->DstAddr, dstAddrStr, sizeof(dstAddrStr));
					}

					if (packetInfo.tcpHdr) {
						srcPort = ntohs(packetInfo.tcpHdr->SrcPort);
						dstPort = ntohs(packetInfo.tcpHdr->DstPort);
					}
					else if (packetInfo.udpHdr) {
						srcPort = ntohs(packetInfo.udpHdr->SrcPort);
						dstPort = ntohs(packetInfo.udpHdr->DstPort);
					}

					const char* outboundStr = m_addr.Outbound ? "Outbound" : "Inbound";
					const char* protocolStr = packetInfo.tcpHdr ? "TCP" : (packetInfo.udpHdr ? "UDP" : "Unknown");
					std::cout << outboundStr << ": " << srcAddrStr << ":" << srcPort << " -> " << dstAddrStr << ":" << dstPort << " (" << protocolStr << ")" << std::endl;
				}

				if (!WinDivertSend(m_handle, m_packetBuffer.data(), m_recvLen, nullptr, &m_addr)) {
					std::cerr << "Failed to send packet." << std::endl;
					return;
				}
			}
		}

		void TrafficEngine::Stop() {
			m_running = false;
		}
	}
}
