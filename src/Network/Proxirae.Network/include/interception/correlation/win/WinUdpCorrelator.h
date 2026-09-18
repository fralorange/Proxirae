#pragma once

#include <unordered_map>

#include "interception/correlation/IPacketCorrelator.h"
#include "interception/correlation/win/WinUdpLookupTable.h"
#include "persistence/associations/AssociationTable.h"
#include "interception/diversion/IPacketContext.h"
#include "interception/diversion/Packet.h"
#include "processes/resolver/IProcessGuard.h"

namespace Proxirae {
	class WinUdpCorrelator : public IPacketCorrelator {
	public:
		WinUdpCorrelator(IProcessGuard& guard, AssociationTable& associations);
		~WinUdpCorrelator() override;

		bool CorrelateNetwork(const Packet& packet, const std::function<void(IPacketContext&)>& callback) override;
		bool CorrelateSocket(const PacketMetadata& metadata, const std::function<void(IPacketContext&)>& callback) override;

	private:
		std::unordered_multimap<FiveTuple, Packet, FiveTupleHash> m_pending;

		IProcessGuard& m_guard;
		AssociationTable& m_associations;

		std::optional<WinUdpLookupTable> m_lookupTable;
	};
}