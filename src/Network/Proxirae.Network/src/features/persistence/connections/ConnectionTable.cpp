#include "features/persistence/connections/ConnectionTable.h"

namespace Proxirae {
	void ConnectionTable::Add(const FiveTuple& key, const ConnectionEntry& entry) {
		std::lock_guard<std::mutex> lock(m_mutex);

		m_connections.insert_or_assign(key, entry);

		ThreeTuple iKey{ key.srcAddress, key.srcPort, key.protocol };
		m_indexes.insert_or_assign(iKey, key);
	}

	void ConnectionTable::Remove(const FiveTuple& key) {
		std::lock_guard<std::mutex> lock(m_mutex);

		m_connections.erase(key);

		ThreeTuple iKey{ key.srcAddress, key.srcPort, key.protocol };
		m_indexes.erase(iKey);
	}

	void ConnectionTable::AddAlias(const ThreeTuple& alias, const FiveTuple& key) {
		std::lock_guard<std::mutex> lock(m_mutex);

		m_indexes.insert_or_assign(alias, key);
	}

	void ConnectionTable::RemoveAlias(const ThreeTuple& alias) {
		std::lock_guard<std::mutex> lock(m_mutex);

		m_indexes.erase(alias);
	}

	std::optional<std::reference_wrapper<const ConnectionEntry>> ConnectionTable::Get(const FiveTuple& key) const {
		std::lock_guard<std::mutex> lock(m_mutex);

		auto it = m_connections.find(key);

		if (it != m_connections.end()) {
			return it->second;
		}

		return std::nullopt;
	}

	std::optional<FiveTuple> ConnectionTable::FindKey(const ThreeTuple& key)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		auto it = m_indexes.find(key);

		if (it != m_indexes.end()) {
			return it->second;
		}

		return std::nullopt;
	}

	bool ConnectionTable::Exists(const FiveTuple& key) const 
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		return m_connections.contains(key);
	}
}
