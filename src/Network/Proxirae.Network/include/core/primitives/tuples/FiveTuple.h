#pragma once

#include <cstdint>

#include "core/primitives/ip/IpAddress.h"

namespace Proxirae {
	struct FiveTuple {
		IpAddress srcAddress{};
		std::uint16_t srcPort = 0;
		IpAddress dstAddress{};
		std::uint16_t dstPort = 0;
		std::uint8_t protocol = 0;

		bool operator==(const FiveTuple&) const = default;
	};
}