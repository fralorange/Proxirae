#include "core/Endpoint.h"
#include <WS2tcpip.h>
#include <format>

namespace Proxirae {
	Endpoint::Endpoint(UINT32 address, UINT16 port) : m_address(address), m_port(port) {}

	std::string Endpoint::ToString() const
	{
		char addrStr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &m_address, addrStr, sizeof(addrStr));

		return std::format("{}:{}", addrStr, ntohs(m_port));
	}
}
