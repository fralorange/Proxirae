#include "persistence/AssociationTable.h"

namespace Proxirae {
	void AssociationTable::AddAssociation(const FiveTuple& key, const AssociationEntry& entry)
	{
		m_associations[key] = entry;
	}

	void AssociationTable::RemoveAssociation(const FiveTuple& key)
	{
		m_associations.erase(key);
	}

	std::optional<AssociationEntry> AssociationTable::GetAssociation(const FiveTuple& key) const
	{
		auto it = m_associations.find(key);
		if (it != m_associations.end()) {
			return it->second;
		}
		return std::nullopt;
	}

	bool AssociationTable::AssociationExists(const FiveTuple& key) const
	{
		return m_associations.contains(key);
	}
}