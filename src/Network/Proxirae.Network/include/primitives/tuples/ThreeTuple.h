#pragma once

#include <cstdint>

#include "primitives/ip/IpAddress.h"

namespace Proxirae {
	struct ThreeTuple {
		IpAddress srcAddress{};
		std::uint16_t srcPort = 0;
		std::uint8_t protocol = 0;

		bool operator==(const ThreeTuple&) const = default;
	};
}