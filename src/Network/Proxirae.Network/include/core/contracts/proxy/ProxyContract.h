#pragma once

#include <string>

#include "core/contracts/proxy/ProxyType.h"

namespace Proxirae {
	struct ProxyContract {
		std::string id;
		std::string address;
		std::uint16_t port;
		ProxyType type;
		std::string username;
		std::string password;
	};
}