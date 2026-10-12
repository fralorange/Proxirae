#pragma once

#include <string>
#include <chrono>

namespace Proxirae {
	struct DomainEntry {
		std::string domain;
		std::chrono::steady_clock::time_point expiresAt;
	};
}