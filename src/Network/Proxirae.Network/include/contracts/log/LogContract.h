#pragma once

#include <string_view>
#include <chrono>

#include "LogLevelContract.h"

namespace Proxirae {
	struct LogContract {
		std::chrono::system_clock::time_point timestamp;
		LogLevelContract level;
		std::string_view message;
	};
}