#pragma once

#include "core/primitives/tuples/FiveTuple.h"

namespace Proxirae {
	struct FiveTupleHash {
		std::size_t operator()(const FiveTuple& key) const;
	};
}