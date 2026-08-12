#pragma once

#include <cstddef>

#include "core/registry/ThreeTuple.h"

namespace Proxirae {
	struct ThreeTupleHash {
		std::size_t operator()(const ThreeTuple& key) const;
	};
}