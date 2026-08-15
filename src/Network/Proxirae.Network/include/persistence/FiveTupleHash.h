#pragma once

#include "persistence/FiveTuple.h"

namespace Proxirae {
	struct FiveTupleHash {
		std::size_t operator()(const FiveTuple& key) const;
	};
}