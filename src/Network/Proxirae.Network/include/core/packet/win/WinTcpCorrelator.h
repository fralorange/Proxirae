#pragma once

#include <unordered_map>

#include "core/registry/AssociationRegistry.h"
#include "core/registry/FiveTuple.h"
#include "core/registry/FiveTupleHash.h"
#include "core/packet/IPacketContext.h"
#include "core/packet/Packet.h"
#include "process/IProcessMonitor.h"

namespace Proxirae {
	class WinTcpCorrelator {
	public:
		WinTcpCorrelator(IProcessMonitor& monitor, AssociationRegistry& associations);

		void CorrelateNetwork(const Packet& packet, const std::function<void(IPacketContext&)>& callback);
		void CorrelateSocket(const PacketMetadata& metadata, const std::function<void(IPacketContext&)>& callback);

	private:
		std::unordered_multimap<FiveTuple, Packet, FiveTupleHash> m_pending;

		IProcessMonitor& m_monitor;
		AssociationRegistry& m_associations;
	};
}