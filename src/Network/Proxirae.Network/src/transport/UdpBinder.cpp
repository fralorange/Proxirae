#include <format>

#include "transport/UdpBinder.h"
#include "environment/sock.h"

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
		NativeSocket sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if (sock == InvalidNativeSocket) {
			m_logger.LogError(std::format("[UdpBinder] Failed to create socket: error {}", GetSocketError()));

			return 0;
		}

		struct sockaddr_in sockAddr;

		std::memset(&sockAddr, 0, sizeof(sockAddr));
		sockAddr.sin_port = 0;
		sockAddr.sin_family = AF_INET;

		if (bind(sock, reinterpret_cast<struct sockaddr*>(&sockAddr), sizeof(sockAddr)) == SocketError) {
			m_logger.LogError(std::format("[UdpBinder] Bind failed: error {}", GetSocketError()));
			CloseSocket(sock);

			return 0;
		}

		struct sockaddr_in boundAddr {};
		NativeSocketLen len = sizeof(boundAddr);
		if (getsockname(sock, reinterpret_cast<struct sockaddr*>(&boundAddr), &len) == SocketError) {
			m_logger.LogError(std::format("[UdpBinder] getsockname failed: error {}", GetSocketError()));
			CloseSocket(sock);
			return 0;
		}

		m_socket = sock;

		m_driver.Attach(m_socket);
		return ntohs(boundAddr.sin_port);
	}

	NativeSocket UdpBinder::ReleaseSocket()
	{
		return std::exchange(m_socket, InvalidNativeSocket);
	}
}