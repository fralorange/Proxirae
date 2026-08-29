#pragma once

#include <unordered_map>

#include "persistence/AssociationTable.h"
#include "persistence/FiveTuple.h"
#include "persistence/FiveTupleHash.h"
#include "interception/IPacketContext.h"
#include "interception/Packet.h"
#include "processes/IProcessGuard.h"

namespace Proxirae {
	class WinTcpCorrelator {
	public:
		WinTcpCorrelator(IProcessGuard& guard, AssociationTable& associations);

		void CorrelateNetwork(const Packet& packet, const std::function<void(IPacketContext&)>& callback);
		void CorrelateSocket(const PacketMetadata& metadata, const std::function<void(IPacketContext&)>& callback);

	private:
		std::unordered_multimap<FiveTuple, Packet, FiveTupleHash> m_pending;

		IProcessGuard& m_guard;
		AssociationTable& m_associations;
	};
}