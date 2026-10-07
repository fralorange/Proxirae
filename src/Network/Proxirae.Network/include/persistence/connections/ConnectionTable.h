#pragma once

#include <unordered_map>
#include <optional>
#include <string>
#include <mutex>
#include <functional>
#include <string_view>

#include "primitives/tuples/FiveTuple.h"
#include "primitives/tuples/FiveTupleHash.h"
#include "primitives/tuples/ThreeTuple.h"
#include "primitives/tuples/ThreeTupleHash.h"
#include "persistence/connections/ConnectionEntry.h"

namespace Proxirae {
	class ConnectionTable {
	public:
		void Add(const FiveTuple& key, const ConnectionEntry& entry);
		void Remove(const FiveTuple& key);

		void AddAlias(const ThreeTuple& alias, const FiveTuple& key);
		void RemoveAlias(const ThreeTuple& alias);

		std::optional<std::reference_wrapper<const ConnectionEntry>> Get(const FiveTuple& key) const;

		std::optional<FiveTuple> FindKey(const ThreeTuple& key);

		bool Exists(const FiveTuple& key) const;

	private:
		mutable std::mutex m_mutex;

		std::unordered_map<FiveTuple, ConnectionEntry, FiveTupleHash> m_connections;
		std::unordered_map<ThreeTuple, FiveTuple, ThreeTupleHash> m_indexes;
	};
}
