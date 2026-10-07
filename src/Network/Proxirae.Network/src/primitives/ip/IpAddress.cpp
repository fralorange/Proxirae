#include <cstdint>

#include "primitives/ip/IpAddress.h"
#include "environment/inet.h"

namespace Proxirae {
	bool IpAddress::IsAny() const
	{
		if (isIPv6) {
			for (auto b : data) {
				if (b != 0) {
					return false;
				}
			}
			return true;
		}

		return data[0] == 0;
	}

	std::string IpAddress::ToString() const {
		char buffer[INET6_ADDRSTRLEN] = { 0 };

		if (isIPv6) {
			std::uint32_t nboData[4]{};
			nboData[0] = htonl(data[0]);
			nboData[1] = htonl(data[1]);
			nboData[2] = htonl(data[2]);
			nboData[3] = htonl(data[3]);

			inet_ntop(AF_INET6, nboData, buffer, sizeof(buffer));
		}
		else {
			std::uint32_t nboData = htonl(data[0]);
			inet_ntop(AF_INET, &nboData, buffer, sizeof(buffer));
		}

		return std::string(buffer);
	}
}