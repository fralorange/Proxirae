#pragma once

#include <unordered_map>
#include <optional>

#include "core/registry/FiveTuple.h"
#include "core/registry/FiveTupleHash.h"
#include "core/registry/AssociationEntry.h"

namespace Proxirae {
	class AssociationRegistry {
	public:
		void AddAssociation(const FiveTuple& key, const AssociationEntry& entry);
		void RemoveAssociation(const FiveTuple& key);

		std::optional<AssociationEntry> GetAssociation(const FiveTuple& key) const;

		bool AssociationExists(const FiveTuple& key) const;

	private:
		std::unordered_map<FiveTuple, AssociationEntry, FiveTupleHash> m_associations;
	};
}