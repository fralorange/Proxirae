#include <string>
#include <format>

#include "environment/sock.h"
#include "transport/stream/TcpListener.h"
#include "primitives/endpoints/Endpoint.h"
#include "utils/SocketUtils.h"

namespace Proxirae {
	TcpListener::TcpListener(IAsyncDriver& driver, IIoStreamAdapter& adapter, ILogger& logger, IProxyFactory& factory)
		: m_driver(driver), m_adapter(adapter), m_logger(logger), m_proxyFactory(factory) {}

	TcpListener::~TcpListener()
	{
		Close();
	}

	std::uint16_t TcpListener::Bind()
	{
		NativeSocket listener = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);

		if (listener == InvalidNativeSocket) {
			m_logger.LogError(std::format("[TcpListener] Failed to create socket: error {}", GetSocketError()));

			return 0;
		}

		int v6only = 0;
		if (setsockopt(listener, IPPROTO_IPV6, IPV6_V6ONLY, reinterpret_cast<const char*>(&v6only), sizeof(v6only)) == SocketError) {
			m_logger.LogError(std::format("[TcpListener] Failed to set IPV6_V6ONLY=0: error {}", GetSocketError()));
			CloseSocket(listener);
			
			return 0;
		}

		int reuse = 1;
		if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&reuse), sizeof(int))) {
			m_logger.LogWarning(std::format("[TcpListener] Failed to set SO_REUSEADDR: error {}", GetSocketError()));
		}

		struct sockaddr_in6 listenerAddr{};
		listenerAddr.sin6_family = AF_INET6;
		listenerAddr.sin6_addr = in6addr_any;
		listenerAddr.sin6_port = 0;

		if (bind(listener, reinterpret_cast<struct sockaddr*>(&listenerAddr), sizeof(listenerAddr)) == SocketError) {
			m_logger.LogError(std::format("[TcpListener] Bind failed: error {}", GetSocketError()));
			CloseSocket(listener);

			return 0;
		}

		struct sockaddr_in6 boundAddr {};
		NativeSocketLen len = sizeof(boundAddr);
		if (getsockname(listener, reinterpret_cast<struct sockaddr*>(&boundAddr), &len) == SocketError) {
			m_logger.LogError(std::format("[TcpListener] getsockname failed: error {}", GetSocketError()));
			CloseSocket(listener);

			return 0;
		}

		m_listener = listener;

		return ntohs(boundAddr.sin6_port);
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
		struct sockaddr_in6 clientAddr{};
		NativeSocketLen clientAddrSize = sizeof(clientAddr);

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

		IpAddress ip = SocketUtils::FromSockAddr(clientAddr);
		std::uint16_t port = ntohs(clientAddr.sin6_port);

		Endpoint endpoint(ip, port);

		m_logger.LogDebug(std::format("[TcpListener] Client connected from {}", endpoint.ToString()));

		return std::make_shared<TcpSession>(client, endpoint, m_adapter, m_logger, m_proxyFactory);
	}

	void TcpListener::Close() {
		if (m_listener != InvalidNativeSocket) {
			CloseSocket(m_listener);
			m_listener = InvalidNativeSocket;
		}
	}
}