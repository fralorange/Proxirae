#include "Persistence/ConnectionTable.h"

namespace Proxirae::Network::Persistence {
	void ConnectionTable::AddConnection(const ConnectionKey& key, const ConnectionEntry& entry) {
		m_connections[key] = entry;
	}

	void ConnectionTable::RemoveConnection(const ConnectionKey& key) {
		m_connections.erase(key);
	}

	std::optional<ConnectionEntry> ConnectionTable::GetConnection(const ConnectionKey& key) const {
		auto it = m_connections.find(key);
		if (it != m_connections.end()) {
			return it->second;
		}
		return std::nullopt;
	}
}