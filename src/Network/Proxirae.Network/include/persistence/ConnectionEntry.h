#pragma once

#include <optional>

namespace Proxirae {
	struct ConnectionEntry {
		std::uint64_t createdAt = 0;
		std::uint64_t lastSeen = 0;
		
		std::optional<std::string> proxyId;
		std::optional<std::uint32_t> processId;
	};
}