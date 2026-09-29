#pragma once

#include <string>
#include <cstdint>

#include "primitives/ip/IpAddress.h"

namespace Proxirae {
	class Endpoint {
	public:
		Endpoint(const IpAddress& address, std::uint16_t port);
		
		IpAddress GetAddress() const;
		std::uint16_t GetPort() const;

		std::string ToString() const;
	private:
		IpAddress m_address;
		std::uint16_t m_port;
	};
}