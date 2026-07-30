#include <WS2tcpip.h>
#include <format>
#include <thread>

#include "core/TcpSession.h"
#include "core/Endpoint.h"
#include "core/Socks5Proxy.h"

namespace Proxirae {
	TcpSession::TcpSession(SOCKET client, SOCKADDR_IN address, ILogger& logger)
		: m_client(client), m_address(address), m_logger(logger) { }

	TcpSession::~TcpSession() {
		Stop();
	}

	UINT32 TcpSession::GetAddress() const
	{
		return m_address.sin_addr.s_addr;
	}

	UINT16 TcpSession::GetPort() const
	{
		return m_address.sin_port;
	}

	int SendAll(SOCKET sock, const char* buffer, int length)
	{
		int totalSent = 0;
		while (totalSent < length) {
			int bytes = send(sock, buffer + totalSent, length - totalSent, 0);
			if (bytes <= 0) {
				return -1;
			}

			totalSent += bytes;
		}

		return totalSent;
	}

	void TcpSession::Handle(ConnectionEntry entry)
	{
		Socks5Proxy proxy("127.0.0.1", 10808, m_logger);

		char targetHost[INET_ADDRSTRLEN];
		int targetPort;

		inet_ntop(AF_INET, &entry.destAddress, targetHost, sizeof(targetHost));
		targetPort = ntohs(entry.destPort);

		if (!proxy.Connect(targetHost, targetPort)) {
			shutdown(m_client, SD_BOTH);
			closesocket(m_client);
			return;
		}

		std::thread clientToProxy([&proxy, this]() {
			char buffer[4096];

			std::string message;

			while (true) {
				int received = recv(m_client, buffer, sizeof(buffer), 0);

				if (received == 0) {
					m_logger.LogDebug("Terminated DAEMON connection.");
					break;
				}

				if (received == SOCKET_ERROR) {
					if (m_isStopping) {
						m_logger.LogDebug("Session being stopped by Daemon.");
					}
					else {
						message = std::format("Failed to receive data from DAEMON. Error={}", WSAGetLastError());
						m_logger.LogError(message);
					}

					break;
				}

				std::string message = std::format("Received data from DAEMON: Length={}", received);
				m_logger.LogInfo(message);

				if (proxy.Send(buffer, received) <= 0) {
					message = std::format("Failed to send data to PROXY. Error={}", WSAGetLastError());
					m_logger.LogError(message);
					break;
				}

				m_logger.LogInfo("Sent data from DAEMON to PROXY");
			}
		});

		std::thread proxyToClient([&proxy, this]() {
			char buffer[4096];

			std::string message;

			while (true) {
				int received = proxy.Recv(buffer, sizeof(buffer));

				if (received == 0) {
					m_logger.LogDebug("Terminated PROXY connection");
					break;
				}

				if (received == SOCKET_ERROR) {
					message = std::format("Failed to receive data from PROXY. Error={}", WSAGetLastError());
					m_logger.LogError(message);
					break;
				}

				std::string message = std::format("Received data from PROXY: Length={}", received);
				m_logger.LogInfo(message);

				if (SendAll(m_client, buffer, received) <= 0) {
					message = std::format("Failed to send data to CLIENT. Error={}", WSAGetLastError());
					m_logger.LogError(message);
					break;
				}

				m_logger.LogInfo("Sent data from PROXY to CLIENT");
			}
		});

		clientToProxy.join();
		proxyToClient.join();

		shutdown(m_client, SD_BOTH);
		closesocket(m_client);
		proxy.Disconnect();
	}

	void TcpSession::Stop() {
		m_isStopping = true;

		if (m_client != INVALID_SOCKET) {
			shutdown(m_client, SD_BOTH);
			closesocket(m_client);
			m_client == INVALID_SOCKET;
		}
	}
}