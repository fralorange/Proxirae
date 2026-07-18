#pragma once

#include <basetsd.h>

namespace Proxirae::Network::Persistence {
	struct ConnectionKey {
		UINT32 srcAddress = 0;
		UINT16 srcPort = 0;
		UINT8 protocol = 0;

		bool operator==(const ConnectionKey&) const = default;
	};
}