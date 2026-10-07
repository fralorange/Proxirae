#include <functional>

#include "primitives/tuples/ThreeTupleHash.h"
#include "primitives/ip/IpAddressHash.h"

namespace Proxirae {
	std::size_t ThreeTupleHash::operator()(const ThreeTuple& key) const
	{
		IpAddressHash ipHasher;

		std::size_t h1 = ipHasher(key.srcAddress);
		std::size_t h2 = std::hash<std::uint16_t>{}(key.srcPort);
		std::size_t h3 = std::hash<std::uint8_t>{}(key.protocol);
		return h1 ^ (h2 << 1) ^ (h3 << 2);
	}
}