#include <functional>

#include "persistence/FiveTupleHash.h"

namespace Proxirae {
	std::size_t FiveTupleHash::operator()(const FiveTuple& key) const
	{
		std::size_t h1 = std::hash<std::uint32_t>{}(key.srcAddress);
		std::size_t h2 = std::hash<std::uint16_t>{}(key.srcPort);
		std::size_t h3 = std::hash<std::uint32_t>{}(key.dstAddress);
		std::size_t h4 = std::hash<std::uint16_t>{}(key.dstPort);
		std::size_t h5 = std::hash<std::uint8_t>{}(key.protocol);
		return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ (h5 << 4);
	}
}