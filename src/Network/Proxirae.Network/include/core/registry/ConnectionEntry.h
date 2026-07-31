#pragma once

#include "core/registry/ConnectionState.h"

namespace Proxirae {
	struct ConnectionEntry {
		std::uint32_t destAddress = 0;
		std::uint16_t destPort = 0;
		std::uint64_t createdAt = 0;
		std::uint64_t lastSeen = 0;
		ConnectionState state = ConnectionState::NEW;
	};
}