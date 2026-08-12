#include "core/registry/AssociationRegistry.h"

namespace Proxirae {
	void AssociationRegistry::AddAssociation(const FiveTuple& key, const AssociationEntry& entry)
	{
		m_associations[key] = entry;
	}

	void AssociationRegistry::RemoveAssociation(const FiveTuple& key)
	{
		m_associations.erase(key);
	}

	std::optional<AssociationEntry> AssociationRegistry::GetAssociation(const FiveTuple& key) const
	{
		auto it = m_associations.find(key);
		if (it != m_associations.end()) {
			return it->second;
		}
		return std::nullopt;
	}

	bool AssociationRegistry::AssociationExists(const FiveTuple& key) const
	{
		return m_associations.contains(key);
	}
}