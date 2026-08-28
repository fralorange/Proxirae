#pragma once

#include <string>
#include <cstdint>

namespace Proxirae {
	class Endpoint {
	public:
		Endpoint(std::uint32_t address, std::uint16_t port);
		
		std::uint32_t GetAddress() const;
		std::uint16_t GetPort() const;

		std::string ToString() const;
	private:
		std::uint32_t m_address;
		std::uint16_t m_port;
	};
}