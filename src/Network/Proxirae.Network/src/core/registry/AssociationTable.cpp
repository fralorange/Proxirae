#include "core/registry/AssociationTable.h"

namespace Proxirae {
	void AssociationTable::AddAssociation(const AssociationKey& key, const AssociationEntry& entry)
	{
		m_associations[key] = entry;
	}

	void AssociationTable::RemoveAssociation(const AssociationKey& key)
	{
		m_associations.erase(key);
	}

	std::optional<AssociationEntry> AssociationTable::GetAssociation(const AssociationKey& key) const
	{
		auto it = m_associations.find(key);
		if (it != m_associations.end()) {
			return it->second;
		}
		return std::nullopt;
	}

	bool AssociationTable::AssociationExists(const AssociationKey& key) const
	{
		return m_associations.contains(key);
	}
}