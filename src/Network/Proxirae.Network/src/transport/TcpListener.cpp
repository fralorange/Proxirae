#include <string>
#include <format>

#include "platform/sock.h"
#include "transport/TcpListener.h"
#include "packet/Endpoint.h"

namespace Proxirae {
	TcpListener::TcpListener(IIoDriver& driver, ILogger& logger, IProxyFactory& factory)
		: m_driver(driver), m_logger(logger), m_proxyFactory(factory) {}

	TcpListener::~TcpListener()
	{
		if (m_listener != InvalidNativeSocket) {
			CloseSocket(m_listener);
			m_listener = InvalidNativeSocket;
		}
	}

	std::uint16_t TcpListener::Bind()
	{
		NativeSocket listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (listener == InvalidNativeSocket) {
			m_logger.LogCritical("Failed to create socket listener.");

			return 0;
		}

		struct sockaddr_in listenerAddr;

		std::memset(&listenerAddr, 0, sizeof(listenerAddr));
		listenerAddr.sin_port = 0;
		listenerAddr.sin_family = AF_INET;

		int on = 1;
		if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&on), sizeof(int))) {
			std::string message = std::format("Failed to re-use address. Error={}", GetSocketError());
			m_logger.LogWarning(message);
		}

		if (bind(listener, reinterpret_cast<struct sockaddr*>(&listenerAddr), sizeof(listenerAddr)) == SocketError) {
			std::string message = std::format("Failed to bind address to socket. Error={}", GetSocketError());
			m_logger.LogError(message);
			CloseSocket(listener);

			return 0;
		}

		struct sockaddr_in boundAddr {};
		SocketLen len = sizeof(boundAddr);
		if (getsockname(listener, reinterpret_cast<struct sockaddr*>(&boundAddr), &len) == SocketError) {
			std::string message = std::format("Failed to get socket name. Error={}", GetSocketError());
			m_logger.LogError(message);
			CloseSocket(listener);
			return 0;
		}

		m_listener = listener;

		return ntohs(boundAddr.sin_port);
	}

	bool TcpListener::Listen(std::uint16_t port)
	{
		if (listen(m_listener, SOMAXCONN) == SocketError) {
			std::string message = std::format("Failed to listen on socket. Error={}", GetSocketError());
			m_logger.LogError(message);
			CloseSocket(m_listener);

			return false;
		}

		std::string message = std::format("Listening on Port={}", port);
		m_logger.LogDebug(message);

		return true;
	}

	std::shared_ptr<TcpSession> TcpListener::Accept()
	{
		struct sockaddr_in clientAddr{};
		SocketLen clientAddrSize = sizeof(clientAddr);

		NativeSocket client = accept(m_listener, reinterpret_cast<struct sockaddr*>(&clientAddr), &clientAddrSize);

		if (client == InvalidNativeSocket) {
			m_logger.LogError("Failed to accept client connection.");

			return nullptr;
		}

		if (!m_driver.Attach(client)) {
			m_logger.LogError("Failed to attach client socket to IOCP");
			CloseSocket(client);

			return nullptr;
		}

		Endpoint endpoint(clientAddr.sin_addr.s_addr, clientAddr.sin_port);

		std::string message = std::format("Client connected: Src={}", endpoint.ToString());
		m_logger.LogInfo(message);

		return std::make_shared<TcpSession>(client, endpoint, m_driver, m_logger, m_proxyFactory);
	}
}