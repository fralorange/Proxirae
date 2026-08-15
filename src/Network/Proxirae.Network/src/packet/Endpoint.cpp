#include <format>

#include "platform/inet.h"
#include "packet/Endpoint.h"

namespace Proxirae {
	// address and port must be in network byte order
	Endpoint::Endpoint(std::uint32_t address, std::uint16_t port) : m_address(address), m_port(port) {}

	std::uint32_t Endpoint::GetAddress() const {
		return m_address;
	}

	std::uint16_t Endpoint::GetPort() const {
		return m_port;
	}

	std::string Endpoint::ToString() const
	{
		char addrStr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &m_address, addrStr, sizeof(addrStr));

		return std::format("{}:{}", addrStr, ntohs(m_port));
	}
}
