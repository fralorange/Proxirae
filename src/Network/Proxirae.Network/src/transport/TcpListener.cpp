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
		Close();
	}

	std::uint16_t TcpListener::Bind()
	{
		NativeSocket listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (listener == InvalidNativeSocket) {
			m_logger.LogError(std::format("[TcpListener] Failed to create socket: error {}", GetSocketError()));

			return 0;
		}

		struct sockaddr_in listenerAddr;

		std::memset(&listenerAddr, 0, sizeof(listenerAddr));
		listenerAddr.sin_port = 0;
		listenerAddr.sin_family = AF_INET;

		int on = 1;
		if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&on), sizeof(int))) {
			m_logger.LogWarning(std::format("[TcpListener] Failed to set SO_REUSEADDR: error {}", GetSocketError()));
		}

		if (bind(listener, reinterpret_cast<struct sockaddr*>(&listenerAddr), sizeof(listenerAddr)) == SocketError) {
			m_logger.LogError(std::format("[TcpListener] Bind failed: error {}", GetSocketError()));
			CloseSocket(listener);

			return 0;
		}

		struct sockaddr_in boundAddr {};
		SocketLen len = sizeof(boundAddr);
		if (getsockname(listener, reinterpret_cast<struct sockaddr*>(&boundAddr), &len) == SocketError) {
			m_logger.LogError(std::format("[TcpListener] getsockname failed: error {}", GetSocketError()));
			CloseSocket(listener);
			return 0;
		}

		m_listener = listener;

		return ntohs(boundAddr.sin_port);
	}

	bool TcpListener::Listen(std::uint16_t port)
	{
		if (listen(m_listener, SOMAXCONN) == SocketError) {
			m_logger.LogError(std::format("[TcpListener] Listen failed on port {}: error {}", port, GetSocketError()));
			CloseSocket(m_listener);

			return false;
		}

		m_logger.LogInfo(std::format("[TcpListener] Listening on port {}", port));

		return true;
	}

	std::shared_ptr<TcpSession> TcpListener::Accept()
	{
		struct sockaddr_in clientAddr{};
		SocketLen clientAddrSize = sizeof(clientAddr);

		NativeSocket client = accept(m_listener, reinterpret_cast<struct sockaddr*>(&clientAddr), &clientAddrSize);

		if (client == InvalidNativeSocket) {
			if (m_listener == InvalidNativeSocket) {
				return nullptr;
			}

			m_logger.LogError(std::format("[TcpListener] Accept failed: error {}", GetSocketError()));

			return nullptr;
		}

		if (!m_driver.Attach(client)) {
			m_logger.LogError(std::format("[TcpListener] Failed to attach client socket to IOCP: error {}", GetSocketError()));
			CloseSocket(client);

			return nullptr;
		}

		Endpoint endpoint(clientAddr.sin_addr.s_addr, clientAddr.sin_port);

		m_logger.LogDebug(std::format("[TcpListener] Client connected from {}", endpoint.ToString()));

		return std::make_shared<TcpSession>(client, endpoint, m_driver, m_logger, m_proxyFactory);
	}

	void TcpListener::Close() {
		if (m_listener != InvalidNativeSocket) {
			CloseSocket(m_listener);
			m_listener = InvalidNativeSocket;
		}
	}
}