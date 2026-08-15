#pragma once

#include <unordered_map>

#include "persistence/AssociationTable.h"
#include "persistence/FiveTuple.h"
#include "persistence/FiveTupleHash.h"
#include "packet/IPacketContext.h"
#include "packet/Packet.h"
#include "process/IProcessMonitor.h"

namespace Proxirae {
	class WinTcpCorrelator {
	public:
		WinTcpCorrelator(IProcessMonitor& monitor, AssociationTable& associations);

		void CorrelateNetwork(const Packet& packet, const std::function<void(IPacketContext&)>& callback);
		void CorrelateSocket(const PacketMetadata& metadata, const std::function<void(IPacketContext&)>& callback);

	private:
		std::unordered_multimap<FiveTuple, Packet, FiveTupleHash> m_pending;

		IProcessMonitor& m_monitor;
		AssociationTable& m_associations;
	};
}