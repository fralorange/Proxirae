#include <format>

#include "environment/inet.h"
#include "primitives/endpoints/Endpoint.h"

namespace Proxirae {
	Endpoint::Endpoint(const IpAddress& address, std::uint16_t port) : m_address(address), m_port(port) {}

	IpAddress Endpoint::GetAddress() const {
		return m_address;
	}

	std::uint16_t Endpoint::GetPort() const {
		return m_port;
	}

	std::string Endpoint::ToString() const
	{
		if (!m_address.isIPv6) {
			char addrStr[INET_ADDRSTRLEN];

			std::uint32_t ipNbo = htonl(*reinterpret_cast<const std::uint32_t*>(m_address.data.data()));

			inet_ntop(AF_INET, &ipNbo, addrStr, sizeof(addrStr));

			return std::format("{}:{}", addrStr, m_port);
		}
		else {
			char addrStr[INET6_ADDRSTRLEN];

			std::uint32_t ipNbo[4]{};
			const std::uint32_t* ipHbo = reinterpret_cast<const std::uint32_t*>(m_address.data.data());

			for (int i = 0; i < 4; ++i) {
				ipNbo[i] = htonl(ipHbo[i]);
			}

			inet_ntop(AF_INET6, ipNbo, addrStr, sizeof(addrStr));

			return std::format("[{}]:{}", addrStr, m_port);
		}
	}
}