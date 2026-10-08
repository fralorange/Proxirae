#pragma once

#include <cstddef>

#include "IpAddress.h"

namespace Proxirae {
	struct IpAddressHash {
		std::size_t operator()(const IpAddress& ip) const;
	};
}