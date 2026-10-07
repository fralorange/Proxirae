#pragma once

#include <array>
#include <string>
#include <cstdint>

namespace Proxirae {
	struct IpAddress {
		std::array<std::uint32_t, 4> data{ 0 };
		bool isIPv6{ false };

		bool operator==(const IpAddress&) const = default;

		bool IsAny() const;
		std::string ToString() const;
	};
}