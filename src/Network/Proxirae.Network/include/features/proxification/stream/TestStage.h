#pragma once

#include <cstdint>

namespace Proxirae {
	enum class TestStage : std::uint8_t {
		Failed,
		Establish,
		Handshake,
		Connect
	};
}