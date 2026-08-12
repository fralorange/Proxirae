#pragma once

#include <cstdint>

namespace Proxirae {
	enum class RuleAction : std::uint8_t {
		Direct,
		Block,
		Proxy
	};
}