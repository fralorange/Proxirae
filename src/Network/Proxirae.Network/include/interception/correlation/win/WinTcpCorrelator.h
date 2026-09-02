#pragma once

#include <unordered_map>

#include "interception/correlation/IPacketCorrelator.h"
#include "persistence/AssociationTable.h"
#include "persistence/FiveTuple.h"
#include "persistence/FiveTupleHash.h"
#include "interception/diversion/IPacketContext.h"
#include "interception/diversion/Packet.h"
#include "processes/IProcessGuard.h"

namespace Proxirae {
	class WinTcpCorrelator : public IPacketCorrelator {
	public:
		WinTcpCorrelator(IProcessGuard& guard, AssociationTable& associations);

		bool CorrelateNetwork(const Packet& packet, const std::function<void(IPacketContext&)>& callback) override;
		bool CorrelateSocket(const PacketMetadata& metadata, const std::function<void(IPacketContext&)>& callback) override;

	private:
		std::unordered_multimap<FiveTuple, Packet, FiveTupleHash> m_pending;

		IProcessGuard& m_guard;
		AssociationTable& m_associations;
	};
}