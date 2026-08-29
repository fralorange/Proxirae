#pragma once

#include <cstddef>

#include "persistence/ThreeTuple.h"

namespace Proxirae {
	struct ThreeTupleHash {
		std::size_t operator()(const ThreeTuple& key) const;
	};
}