#include <format>

#include "features/transport/datagram/UdpBinder.h"
#include "platform/environment/sock.h"

namespace Proxirae {
	UdpBinder::UdpBinder(IAsyncDriver& driver, ILogger& logger)
		: m_driver(driver), m_logger(logger) { }

	UdpBinder::~UdpBinder()
	{
		if (m_socket != InvalidNativeSocket) {
			CloseSocket(m_socket);
		}
	}

	std::uint16_t UdpBinder::Bind(std::uint16_t requestedPort)
	{
		NativeSocket sock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);

		if (sock == InvalidNativeSocket) {
			m_logger.LogError(std::format("[UdpBinder] Failed to create socket: error {}", GetSocketError()));

			return 0;
		}

		int v6only = 0;
		if (setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, reinterpret_cast<const char*>(&v6only), sizeof(v6only)) == SocketError) {
			m_logger.LogError(std::format("[UdpBinder] Failed to set IPV6_V6ONLY=0: error {}", GetSocketError()));
			CloseSocket(sock);

			return 0;
		}

		int dontFragment = 0;
		if (setsockopt(sock, IPPROTO_IP, IP_DONTFRAGMENT, reinterpret_cast<const char*>(&dontFragment), sizeof(dontFragment)) == SocketError) {
			m_logger.LogError(std::format("[UdpBinder] Failed to set IP_DONTFRAGMENT=0: error {}", GetSocketError()));
			CloseSocket(sock);

			return 0;
		}

		if (setsockopt(sock, IPPROTO_IPV6, IPV6_DONTFRAG, reinterpret_cast<const char*>(&dontFragment), sizeof(dontFragment)) == SocketError) {
			m_logger.LogError(std::format("[UdpBinder] Failed to set IPV6_DONTFRAG=0: error {}", GetSocketError()));
			CloseSocket(sock);

			return 0;
		}

		struct sockaddr_in6 sockAddr{};
		sockAddr.sin6_family = AF_INET6;
		sockAddr.sin6_port = htons(requestedPort);
		sockAddr.sin6_addr = in6addr_any;

		if (bind(sock, reinterpret_cast<struct sockaddr*>(&sockAddr), sizeof(sockAddr)) == SocketError) {
			m_logger.LogError(std::format("[UdpBinder] Bind failed: error {}", GetSocketError()));
			CloseSocket(sock);

			return 0;
		}

		struct sockaddr_in6 boundAddr {};
		NativeSocketLen len = sizeof(boundAddr);
		if (getsockname(sock, reinterpret_cast<struct sockaddr*>(&boundAddr), &len) == SocketError) {
			m_logger.LogError(std::format("[UdpBinder] getsockname failed: error {}", GetSocketError()));
			CloseSocket(sock);
			return 0;
		}

		m_socket = sock;
		m_driver.Attach(m_socket);

		return ntohs(boundAddr.sin6_port);
	}

	NativeSocket UdpBinder::ReleaseSocket()
	{
		return std::exchange(m_socket, InvalidNativeSocket);
	}
}