#pragma once

#include <unordered_map>
#include <optional>
#include <string>
#include <functional>
#include <string_view>

#include "core/registry/FiveTuple.h"
#include "core/registry/FiveTupleHash.h"
#include "core/registry/ThreeTuple.h"
#include "core/registry/ThreeTupleHash.h"
#include "core/registry/ConnectionEntry.h"

namespace Proxirae {
	class ConnectionRegistry {
	public:
		void AddConnection(const FiveTuple& key, const ConnectionEntry& entry);
		void RemoveConnection(const FiveTuple& key);

		std::optional<std::reference_wrapper<const ConnectionEntry>> GetConnection(const FiveTuple& key) const;

		std::optional<ConnectionState> GetState(const FiveTuple& key) const;
		std::optional<std::string_view> GetProxyId(const FiveTuple& key) const;

		void SetState(const FiveTuple& key, ConnectionState state);
		void SetProxyId(const FiveTuple& key, const std::string proxyId);

		std::optional<FiveTuple> FindKey(const ThreeTuple& key);

		bool ConnectionExists(const FiveTuple& key) const;

	private:
		std::unordered_map<FiveTuple, ConnectionEntry, FiveTupleHash> m_connections;
		std::unordered_map<ThreeTuple, FiveTuple, ThreeTupleHash> m_indexes;
	};
}