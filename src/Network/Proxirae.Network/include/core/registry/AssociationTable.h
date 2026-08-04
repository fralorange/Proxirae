#pragma once

#include <unordered_map>
#include <optional>

#include "core/registry/AssociationKey.h"
#include "core/registry/AssociationEntry.h"
#include "core/registry/AssociationKeyHash.h"

namespace Proxirae {
	class AssociationTable {
	public:
		void AddAssociation(const AssociationKey& key, const AssociationEntry& entry);
		void RemoveAssociation(const AssociationKey& key);

		std::optional<AssociationEntry> GetAssociation(const AssociationKey& key) const;

		bool AssociationExists(const AssociationKey& key) const;

	private:
		std::unordered_map<AssociationKey, AssociationEntry, AssociationKeyHash> m_associations;
	};
}