#pragma once

#include <string>
#include <cstdint>
#include <optional>

namespace Proxirae {
	struct TestProgressContract {
		std::string id;
		std::uint8_t stage; 
		std::optional<std::uint64_t> latency;
	};
}