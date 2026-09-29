#include "utils/SocketUtils.h"

namespace Proxirae::SocketUtils {
	IpAddress FromSockAddr(const sockaddr_in6& addr) {
		IpAddress ip{};

		if (IN6_IS_ADDR_V4MAPPED(&addr.sin6_addr)) {
			ip.isIPv6 = false;
			std::uint32_t netIpv4 = 0;
			std::memcpy(&netIpv4, &addr.sin6_addr.s6_bytes[12], 4);
			ip.data[0] = ntohl(netIpv4);
		}
		else {
			ip.isIPv6 = true;
			std::array<std::uint32_t, 4> netWords{};
			std::memcpy(netWords.data(), &addr.sin6_addr, 16);

			for (std::size_t i = 0; i < 4; ++i) {
				ip.data[i] = ntohl(netWords[i]);
			}
		}

		return ip;
	}

	IpAddress FromSockAddr(const sockaddr_in& addr)
	{
		IpAddress ip{};
		ip.isIPv6 = false;
		ip.data[0] = ntohl(addr.sin_addr.s_addr);
		return ip;
	}

	int ToSockAddr(const Endpoint& endpoint, sockaddr_storage& outStorage)
	{
		std::memset(&outStorage, 0, sizeof(outStorage));
		const IpAddress& ip = endpoint.GetAddress();
		std::uint16_t netPort = htons(endpoint.GetPort());

		if (!ip.isIPv6) {
			auto* addr4 = reinterpret_cast<sockaddr_in*>(&outStorage);
			addr4->sin_family = AF_INET;
			addr4->sin_port = netPort;
			addr4->sin_addr.s_addr = htonl(ip.data[0]); 

			return sizeof(sockaddr_in);
		}
		else {
			auto* addr6 = reinterpret_cast<sockaddr_in6*>(&outStorage);
			addr6->sin6_family = AF_INET6;
			addr6->sin6_port = netPort;

			for (std::size_t i = 0; i < 4; ++i) {
				std::uint32_t netWord = htonl(ip.data[i]);
				std::memcpy(&addr6->sin6_addr.s6_bytes[i * 4], &netWord, 4);
			}

			return sizeof(sockaddr_in6);
		}
	}
}