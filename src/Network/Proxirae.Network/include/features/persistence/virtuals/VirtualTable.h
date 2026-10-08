#pragma once

#include <cstdint>
#include <optional>
#include <mutex>
#include <queue>
#include <unordered_map>

#include "VirtualEntry.h"
#include "core/primitives/tuples/ThreeTuple.h"
#include "core/primitives/tuples/ThreeTupleHash.h"
#include "core/primitives/tuples/FiveTupleHash.h"

namespace Proxirae {
	class VirtualTable {
	public:
		explicit VirtualTable(std::uint16_t startPort = 60000, std::uint16_t endPort = 65000);
		~VirtualTable() = default;

		std::uint16_t FindOrAdd(const VirtualEntry& entry);
		void Remove(const ThreeTuple& key);
		
		std::optional<VirtualEntry> Resolve(const ThreeTuple& key) const;

	private:
		mutable std::mutex m_mutex;

		std::queue<std::uint16_t> m_freePorts;

		std::unordered_map<FiveTuple, std::uint16_t, FiveTupleHash> m_realToVirtual;
		std::unordered_map<ThreeTuple, VirtualEntry, ThreeTupleHash> m_virtualToReal;
	};
}