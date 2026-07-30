#pragma once

#include <unordered_map>
#include <optional>

#include "ConnectionKey.h"
#include "ConnectionKeyHash.h"
#include "ConnectionEntry.h"

namespace Proxirae {
	class ConnectionTable {
	public:
		void AddConnection(const ConnectionKey& key, const ConnectionEntry& entry);
		void RemoveConnection(const ConnectionKey& key);

		std::optional<ConnectionEntry> GetConnection(const ConnectionKey& key) const;
		std::optional<ConnectionState> GetState(const ConnectionKey& key) const;

		void SetState(const ConnectionKey& key, ConnectionState state);

		bool ConnectionExists(const ConnectionKey& key);

	private:
		std::unordered_map<ConnectionKey, ConnectionEntry, ConnectionKeyHash> m_connections;
	};
}