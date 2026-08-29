#pragma once

#include <chrono>
#include <cstdint>
#include <string>

namespace Proxirae {
	struct RouteContract {
		std::chrono::system_clock::time_point timestamp;
		std::int64_t processId;
		std::uint32_t address;
		std::uint16_t port;
		std::string ruleId;
	};
}