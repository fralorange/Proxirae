#include <string>
#include <format>

#include "core/TcpListener.h"
#include "core/Endpoint.h"

namespace Proxirae {
	TcpListener::TcpListener(ILogger& logger) 
		: m_logger(logger) {}

	TcpListener::~TcpListener()
	{
		if (m_listener != INVALID_SOCKET) {
			closesocket(m_listener);
			m_listener = INVALID_SOCKET;
		}
	}

	bool TcpListener::Listen(UINT16 port)
	{
		SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (listener == INVALID_SOCKET) {
			m_logger.LogCritical("Failed to create DAEMON socket listener.");

			return false;
		}

		SOCKADDR_IN listenerAddr;
		int sizeofListenerAddr = sizeof(listenerAddr);

		std::memset(&listenerAddr, 0, sizeof(listenerAddr));
		listenerAddr.sin_port = htons(port);
		listenerAddr.sin_family = AF_INET;

		int on = 1;
		if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&on), sizeof(int))) {
			std::string message = std::format("Failed to re-use address. Error={}", WSAGetLastError());
			m_logger.LogWarning(message);
		}

		if (bind(listener, reinterpret_cast<SOCKADDR*>(&listenerAddr), sizeofListenerAddr) == SOCKET_ERROR) {
			std::string message = std::format("Failed to bind address to socket. Error={}", WSAGetLastError());
			m_logger.LogError(message);
			closesocket(listener);

			return false;
		}

		if (listen(listener, SOMAXCONN) == SOCKET_ERROR) {
			std::string message = std::format("Failed to listen on socket. Error={}", WSAGetLastError());
			m_logger.LogError(message);
			closesocket(listener);

			return false;
		}

		std::string message = std::format("Listening on Port={}", port);
		m_logger.LogDebug(message);

		m_listener = listener;

		return true;
	}

	std::shared_ptr<TcpSession> TcpListener::Accept()
	{
		SOCKADDR_IN clientAddr{};
		int clientAddrSize = sizeof(clientAddr);

		SOCKET client = accept(m_listener, reinterpret_cast<SOCKADDR*>(&clientAddr), &clientAddrSize);

		if (client == INVALID_SOCKET) {
			m_logger.LogError("Failed to accept client connection.");

			return nullptr;
		}

		Endpoint endpoint(clientAddr.sin_addr.s_addr, clientAddr.sin_port);

		std::string message = std::format("Client connected: Src={}", endpoint.ToString());
		m_logger.LogInfo(message);

		return std::make_shared<TcpSession>(client, clientAddr, m_logger);
	}
}