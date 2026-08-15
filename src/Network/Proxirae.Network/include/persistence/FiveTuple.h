#pragma once

#include <cstdint>

namespace Proxirae {
	struct FiveTuple {
		std::uint32_t srcAddress = 0;
		std::uint16_t srcPort = 0;
		std::uint32_t dstAddress = 0;
		std::uint16_t dstPort = 0;
		std::uint8_t protocol = 0;

		bool operator==(const FiveTuple&) const = default;
	};
}