#pragma once

#include <unordered_map>
#include <optional>

#include "primitives/tuples/FiveTuple.h"
#include "primitives/tuples/FiveTupleHash.h"
#include "persistence/associations/AssociationEntry.h"

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