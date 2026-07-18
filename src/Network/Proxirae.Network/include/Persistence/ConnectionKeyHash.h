#pragma once
#include <cstddef>
#include "ConnectionKey.h"

namespace Proxirae::Network::Persistence {
	struct ConnectionKeyHash {
		std::size_t operator()(const ConnectionKey& key) const {
			std::size_t h1 = std::hash<UINT32>{}(key.srcAddress);
			std::size_t h2 = std::hash<UINT16>{}(key.srcPort);
			std::size_t h3 = std::hash<UINT8>{}(key.protocol);
			return h1 ^ (h2 << 1) ^ (h3 << 4);
		}
	};
}