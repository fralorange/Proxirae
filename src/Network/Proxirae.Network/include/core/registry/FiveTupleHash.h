#pragma once

#include <cstddef>

#include "core/registry/FiveTuple.h"

namespace Proxirae {
	struct FiveTupleHash {
		std::size_t operator()(const FiveTuple& key) const;
	};
}