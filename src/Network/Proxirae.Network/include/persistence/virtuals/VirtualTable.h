#pragma once

#include <cstdint>
#include <optional>
#include <mutex>
#include <queue>
#include <unordered_map>

#include "VirtualEntry.h"
#include "primitives/tuples/ThreeTuple.h"
#include "primitives/tuples/ThreeTupleHash.h"
#include "primitives/tuples/FiveTupleHash.h"

namespace Proxirae {
	class VirtualTable {
	public:
		explicit VirtualTable(std::uint16_t startPort = 60000, std::uint16_t endPort = 65000);
		~VirtualTable() = default;

		std::uint16_t AddVirtual(const VirtualEntry& entry);
		void RemoveVirtual(const ThreeTuple& key);
		
		std::optional<VirtualEntry> ResolveVirtual(const ThreeTuple& key) const;

	private:
		mutable std::mutex m_mutex;

		std::queue<std::uint16_t> m_freePorts;

		std::unordered_map<FiveTuple, std::uint16_t, FiveTupleHash> m_realToVirtual;
		std::unordered_map<ThreeTuple, VirtualEntry, ThreeTupleHash> m_virtualToReal;
	};
}