#include <string>
#include <format>
#include <WS2tcpip.h>

#include "Daemon.h"

constexpr uint16_t ALT_PORT = 33999;

namespace Proxirae::Network {
	Daemon::Daemon(Persistence::ConnectionTable& connections, Logging::ILogger& logger)
		: m_connections(connections), m_logger(logger) {
		WSAData wsaData;
		WORD DLLVersion = MAKEWORD(2, 2);

		if (WSAStartup(DLLVersion, &wsaData) != 0) {
			m_logger.LogCritical("Failed to initialize Winsock.");
			throw std::runtime_error("WSAStartup failed");
		}

		m_listenSocket = INVALID_SOCKET;

		m_running = false;
	}

	Daemon::~Daemon() {
		Stop();

		WSACleanup();
	}

	void Daemon::Start() {
		m_listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (m_listenSocket == INVALID_SOCKET) {
			m_logger.LogCritical("Failed to create socket.");

			return;
		}

		m_running = true;

		SOCKADDR_IN daemonAddr;
		int sizeofDaemonAddr = sizeof(daemonAddr);

		std::memset(&daemonAddr, 0, sizeof(daemonAddr));
		daemonAddr.sin_port = htons(ALT_PORT);
		daemonAddr.sin_family = AF_INET;

		int on = 1;
		if (setsockopt(m_listenSocket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&on), sizeof(int))) {
			std::string message = std::format("Failed to re-use address: {}", WSAGetLastError());
			m_logger.LogWarning(message);
		}

		if (bind(m_listenSocket, reinterpret_cast<SOCKADDR*>(&daemonAddr), sizeofDaemonAddr) == SOCKET_ERROR) {
			std::string message = std::format("Bind failed: {}", WSAGetLastError());
			m_logger.LogError(message);

			Stop();

			return;
		}

		if (listen(m_listenSocket, SOMAXCONN) == SOCKET_ERROR) {
			std::string message = std::format("Listen failed: {}", WSAGetLastError());
			m_logger.LogError(message);

			Stop();

			return;
		}

		std::string message = std::format("Listening on port: {}", ALT_PORT);
		m_logger.LogDebug(message);

		while (m_running) {
			SOCKADDR_IN clientAddr{};
			int clientAddrSize = sizeof(clientAddr);

			SOCKET client = accept(m_listenSocket, reinterpret_cast<SOCKADDR*>(&clientAddr), &clientAddrSize);

			if (client == INVALID_SOCKET) {
				if (!m_running)
					break;

				m_logger.LogError("Failed to accept client connection.");
				continue;
			}

			m_logger.LogInfo("Client connected");

			m_clients.push_back(client);
		}
	}

	void Daemon::Stop() {
		m_running = false;

		if (m_listenSocket != INVALID_SOCKET) {
			closesocket(m_listenSocket);
			m_listenSocket = INVALID_SOCKET;
		}

		for (auto client : m_clients) {
			shutdown(client, SD_BOTH);
			closesocket(client);
		}

		m_clients.clear();
	}
}