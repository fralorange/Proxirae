#pragma once

#include <unordered_map>
#include <optional>

#include "ConnectionKey.h"
#include "ConnectionKeyHash.h"
#include "ConnectionEntry.h"

namespace Proxirae::Network::Persistence {
	class ConnectionTable {
	public:
		void AddConnection(const ConnectionKey& key, const ConnectionEntry& entry);
		void RemoveConnection(const ConnectionKey& key);
		std::optional<ConnectionEntry> GetConnection(const ConnectionKey& key) const;

	private:
		std::unordered_map<ConnectionKey, ConnectionEntry, ConnectionKeyHash> m_connections;
	};
}