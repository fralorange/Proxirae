#pragma once

#include <unordered_map>
#include <optional>
#include <string>
#include <functional>
#include <string_view>

#include "persistence/FiveTuple.h"
#include "persistence/FiveTupleHash.h"
#include "persistence/ThreeTuple.h"
#include "persistence/ThreeTupleHash.h"
#include "persistence/ConnectionEntry.h"

namespace Proxirae {
	class ConnectionTable {
	public:
		void AddConnection(const FiveTuple& key, const ConnectionEntry& entry);
		void RemoveConnection(const FiveTuple& key);

		std::optional<std::reference_wrapper<const ConnectionEntry>> GetConnection(const FiveTuple& key) const;

		std::optional<std::string_view> GetProxyId(const FiveTuple& key) const;
		void SetProxyId(const FiveTuple& key, const std::string proxyId);

		std::optional<FiveTuple> FindKey(const ThreeTuple& key);

		bool ConnectionExists(const FiveTuple& key) const;

	private:
		std::unordered_map<FiveTuple, ConnectionEntry, FiveTupleHash> m_connections;
		std::unordered_map<ThreeTuple, FiveTuple, ThreeTupleHash> m_indexes;
	};
}