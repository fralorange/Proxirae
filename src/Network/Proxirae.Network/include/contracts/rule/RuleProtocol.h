#pragma once

namespace Proxirae {
	enum class RuleProtocol : std::uint8_t {
		None = 0,
		TCP = 1 << 0,
		UDP = 1 << 1,
	};
}