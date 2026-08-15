#include "persistence/ConnectionTable.h"

namespace Proxirae {
	void ConnectionTable::AddConnection(const FiveTuple& key, const ConnectionEntry& entry) {
		m_connections.insert_or_assign(key, entry);

		ThreeTuple iKey{ key.srcAddress, key.srcPort, key.protocol };
		m_indexes.insert_or_assign(iKey, key);
	}

	void ConnectionTable::RemoveConnection(const FiveTuple& key) {
		m_connections.erase(key);

		ThreeTuple iKey{ key.srcAddress, key.srcPort, key.protocol };
		m_indexes.erase(iKey);
	}

	std::optional<std::reference_wrapper<const ConnectionEntry>> ConnectionTable::GetConnection(const FiveTuple& key) const {
		auto it = m_connections.find(key);

		if (it != m_connections.end()) {
			return it->second;
		}

		return std::nullopt;
	}

	std::optional<ConnectionState> ConnectionTable::GetState(const FiveTuple& key) const
	{
		auto it = m_connections.find(key);

		if (it != m_connections.end()) {
			return it->second.state;
		}

		return std::nullopt;
	}

	std::optional<std::string_view> ConnectionTable::GetProxyId(const FiveTuple& key) const
	{
		auto it = m_connections.find(key);

		if (it != m_connections.end() && it->second.proxyId.has_value()) {
			return it->second.proxyId;
		}

		return std::nullopt;
	}

	void ConnectionTable::SetState(const FiveTuple& key, ConnectionState state)
	{
		auto it = m_connections.find(key);

		if (it != m_connections.end()) {
			it->second.state = state;  
		}
	}

	void ConnectionTable::SetProxyId(const FiveTuple& key, const std::string proxyId)
	{
		auto it = m_connections.find(key);

		if (it != m_connections.end()) {
			it->second.proxyId = proxyId; 
		}
	}

	std::optional<FiveTuple> ConnectionTable::FindKey(const ThreeTuple& key)
	{
		auto it = m_indexes.find(key);

		if (it != m_indexes.end()) {
			return it->second;
		}

		return std::nullopt;
	}

	bool ConnectionTable::ConnectionExists(const FiveTuple& key) const {
		return m_connections.contains(key);
	}
}