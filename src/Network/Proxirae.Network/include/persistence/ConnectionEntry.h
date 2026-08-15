#pragma once

#include <optional>

#include "persistence/ConnectionState.h"

namespace Proxirae {
	struct ConnectionEntry {
		std::uint64_t createdAt = 0;
		std::uint64_t lastSeen = 0;

		ConnectionState state = ConnectionState::NEW;
		
		std::optional<std::string> proxyId;
	};
}