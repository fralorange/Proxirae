#include "features/persistence/virtuals/VirtualTable.h"

namespace Proxirae {
	VirtualTable::VirtualTable(std::uint16_t startPort, std::uint16_t endPort)
	{
		for (std::uint16_t port = startPort; port <= endPort; ++port) {
			m_freePorts.push(port);
		}
	}

	std::uint16_t VirtualTable::FindOrAdd(const VirtualEntry& entry)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		auto it = m_realToVirtual.find(entry.realTuple);
		if (it != m_realToVirtual.end()) {
			return it->second;
		}

		if (m_freePorts.empty()) {
			return 0;
		}

		std::uint16_t allocatedPort = m_freePorts.front();
		m_freePorts.pop();

		ThreeTuple key{ entry.realTuple.srcAddress, allocatedPort, entry.realTuple.protocol };

		m_realToVirtual[entry.realTuple] = allocatedPort;

		m_virtualToReal[key] = entry;

		return allocatedPort;
	}

	void VirtualTable::Remove(const ThreeTuple& key)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		auto it = m_virtualToReal.find(key);
		if (it != m_virtualToReal.end()) {
			m_freePorts.push(it->first.srcPort);
			m_realToVirtual.erase(it->second.realTuple);
			m_virtualToReal.erase(it);
		}
	}

	std::optional<VirtualEntry> VirtualTable::Resolve(const ThreeTuple& key) const
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		auto it = m_virtualToReal.find(key);
		if (it != m_virtualToReal.end()) {
			return it->second;
		}

		return std::nullopt;
	}
}