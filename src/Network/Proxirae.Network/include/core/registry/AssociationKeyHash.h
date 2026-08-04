#pragma once

#include <cstddef>

#include "core/registry/AssociationKey.h"

namespace Proxirae {
	struct AssociationKeyHash {
		std::size_t operator()(const AssociationKey& key) const;
	};
}