#include "registry/ConnectionTable.h"

namespace Proxirae {
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

	std::optional<ConnectionState> ConnectionTable::GetState(const ConnectionKey& key) const
	{
		auto it = m_connections.find(key);
		if (it != m_connections.end()) {
			return it->second.state;
		}
		return std::nullopt;
	}

	void ConnectionTable::SetState(const ConnectionKey& key, ConnectionState state)
	{
		auto entry = GetConnection(key);

		if (entry.has_value()) {
			entry.value().state = state;
		}
	}

	bool ConnectionTable::ConnectionExists(const ConnectionKey& key) {
		return m_connections.contains(key);
	}
}