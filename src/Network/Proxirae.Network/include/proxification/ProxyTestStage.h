#pragma once

#include <cstdint>

namespace Proxirae {
	enum class ProxyTestStage : std::uint8_t {
		Failed,
		Establish,
		Handshake,
		Connect
	};
}