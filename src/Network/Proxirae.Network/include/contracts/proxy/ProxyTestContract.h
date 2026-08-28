#pragma once

#include <string>
#include <cstdint>

#include "contracts/proxy/ProxyType.h"

namespace Proxirae {
	struct ProxyTestContract {
		std::string address;
		std::uint16_t port;
		ProxyType type;
		std::string username;
		std::string password;
	};
}