#pragma once
#include <cstdint>

namespace Proxirae {
	enum class LoadTarget : std::uint8_t {
		None = 0,
		Proxies = 1 << 0,
		Rules = 1 << 1,
	};

	inline LoadTarget operator|(LoadTarget a, LoadTarget b) {
		return static_cast<LoadTarget>(static_cast<std::uint8_t>(a) | static_cast<std::uint8_t>(b));
	}

	inline LoadTarget operator&(LoadTarget a, LoadTarget b) {
		return static_cast<LoadTarget>(static_cast<std::uint8_t>(a) & static_cast<std::uint8_t>(b));
	}
}