#pragma once

#include <cstdint>

namespace Proxirae {
	struct ThreeTuple {
		std::uint32_t srcAddress = 0;
		std::uint16_t srcPort = 0;
		std::uint8_t protocol = 0;

		bool operator==(const ThreeTuple&) const = default;
	};
}