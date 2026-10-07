#include "persistence/associations/AssociationTable.h"

namespace Proxirae {
	void AssociationTable::Add(const FiveTuple& key, const AssociationEntry& entry)
	{
		m_associations[key] = entry;
	}

	void AssociationTable::Remove(const FiveTuple& key)
	{
		m_associations.erase(key);
	}

	std::optional<AssociationEntry> AssociationTable::Get(const FiveTuple& key) const
	{
		auto it = m_associations.find(key);
		if (it != m_associations.end()) {
			return it->second;
		}
		return std::nullopt;
	}

	bool AssociationTable::Exists(const FiveTuple& key) const
	{
		return m_associations.contains(key);
	}
}