#include "core/primitives/ip/IpAddressHash.h"

namespace Proxirae {
	std::size_t IpAddressHash::operator()(const IpAddress& ip) const
	{
		std::size_t h = std::hash<bool>{}(ip.isIPv6);

		int count = ip.isIPv6 ? 4 : 1;
		for (int i = 0; i < count; ++i) {
			h ^= std::hash<std::uint32_t>{}(ip.data[i]) + 0x9e3779b9 + (h << 6) + (h >> 2);
		}

		return h;
	}
}