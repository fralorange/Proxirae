#pragma once

#include <string_view>
#include <chrono>

#include "LogLevelContract.h"

namespace Proxirae {
	struct LogContract {
		LogLevelContract level;
		std::string_view message;
		std::chrono::system_clock::time_point timestamp;
	};
}