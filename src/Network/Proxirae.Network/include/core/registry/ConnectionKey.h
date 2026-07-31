#pragma once

#include <cstdint>

namespace Proxirae {
	struct ConnectionKey {
		std::uint32_t srcAddress = 0;
		std::uint16_t srcPort = 0;
		std::uint8_t protocol = 0;

		bool operator==(const ConnectionKey&) const = default;
	};
}