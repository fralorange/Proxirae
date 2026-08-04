#pragma once

#include "core/registry/ConnectionKey.h"

namespace Proxirae {
	struct ConnectionKeyHash {
		std::size_t operator()(const ConnectionKey& key) const;
	};
}