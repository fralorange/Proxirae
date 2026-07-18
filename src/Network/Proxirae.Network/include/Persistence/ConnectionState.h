#pragma once

namespace Proxirae::Network::Persistence {
	enum class ConnectionState {
		NEW,
		ACTIVE,
		IDLE,
		CLOSED
	};
}