#pragma once

#include <cstddef>

#include "core/primitives/tuples/ThreeTuple.h"

namespace Proxirae {
	struct ThreeTupleHash {
		std::size_t operator()(const ThreeTuple& key) const;
	};
}