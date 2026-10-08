#pragma once

#include <string>
#include <cstdint>

#include "core/contracts/proxy/ProxyTestContract.h"

namespace Proxirae {
	struct TestContract {
		std::string id;
		ProxyTestContract proxy;
		std::string testAddress;
		std::uint16_t testPort;
	};
}