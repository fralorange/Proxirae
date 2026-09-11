#pragma once

#include <optional>
#include <string>

namespace Proxirae {
	struct ConnectionEntry {
		std::optional<std::string> proxyId;
		std::optional<std::uint32_t> processId;
	};
}