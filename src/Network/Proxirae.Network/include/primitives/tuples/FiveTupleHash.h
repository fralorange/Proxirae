#pragma once

#include "primitives/tuples/FiveTuple.h"

namespace Proxirae {
	struct FiveTupleHash {
		std::size_t operator()(const FiveTuple& key) const;
	};
}