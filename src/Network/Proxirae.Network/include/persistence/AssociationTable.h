#pragma once

#include <unordered_map>
#include <optional>

#include "persistence/FiveTuple.h"
#include "persistence/FiveTupleHash.h"
#include "persistence/AssociationEntry.h"

namespace Proxirae {
	class AssociationTable {
	public:
		void AddAssociation(const FiveTuple& key, const AssociationEntry& entry);
		void RemoveAssociation(const FiveTuple& key);

		std::optional<AssociationEntry> GetAssociation(const FiveTuple& key) const;

		bool AssociationExists(const FiveTuple& key) const;

	private:
		std::unordered_map<FiveTuple, AssociationEntry, FiveTupleHash> m_associations;
	};
}