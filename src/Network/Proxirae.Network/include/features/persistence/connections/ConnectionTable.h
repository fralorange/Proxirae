#pragma once

#include <unordered_map>
#include <optional>
#include <string>
#include <mutex>
#include <functional>
#include <string_view>

#include "core/primitives/tuples/FiveTuple.h"
#include "core/primitives/tuples/FiveTupleHash.h"
#include "core/primitives/tuples/ThreeTuple.h"
#include "core/primitives/tuples/ThreeTupleHash.h"
#include "features/persistence/connections/ConnectionEntry.h"

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
