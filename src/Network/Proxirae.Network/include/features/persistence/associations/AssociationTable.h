#pragma once

#include <unordered_map>
#include <optional>

#include "core/primitives/tuples/FiveTuple.h"
#include "core/primitives/tuples/FiveTupleHash.h"
#include "features/persistence/associations/AssociationEntry.h"

namespace Proxirae {
	class AssociationTable {
	public:
		void Add(const FiveTuple& key, const AssociationEntry& entry);
		void Remove(const FiveTuple& key);

		std::optional<AssociationEntry> Get(const FiveTuple& key) const;

		bool Exists(const FiveTuple& key) const;

	private:
		std::unordered_map<FiveTuple, AssociationEntry, FiveTupleHash> m_associations;
	};
}