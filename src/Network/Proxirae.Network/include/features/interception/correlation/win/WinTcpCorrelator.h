#pragma once

#include <unordered_map>

#include "features/interception/correlation/IPacketCorrelator.h"
#include "features/persistence/associations/AssociationTable.h"
#include "core/primitives/tuples/FiveTuple.h"
#include "core/primitives/tuples/FiveTupleHash.h"
#include "features/interception/diversion/IPacketContext.h"
#include "features/interception/diversion/Packet.h"
#include "platform/processes/resolver/IProcessGuard.h"

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