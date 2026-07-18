#pragma once

#include <basetsd.h>

#include "ConnectionState.h"

namespace Proxirae::Network::Persistence {
	struct ConnectionEntry {
		UINT32 destAddress = 0;
		UINT16 destPort = 0;
		UINT64 createdAt = 0;
		UINT64 lastSeen = 0;
		ConnectionState state = ConnectionState::NEW;
	};
}