#pragma once

#include <unordered_map>

#include "features/interception/correlation/IPacketCorrelator.h"
#include "features/interception/correlation/win/WinUdpLookupTable.h"
#include "features/persistence/associations/AssociationTable.h"
#include "features/interception/diversion/IPacketContext.h"
#include "features/interception/diversion/Packet.h"
#include "platform/processes/resolver/IProcessGuard.h"

namespace Proxirae {
	class WinUdpCorrelator : public IPacketCorrelator {
	public:
		WinUdpCorrelator(IProcessGuard& guard, AssociationTable& associations, std::optional<WinUdpLookupTable> lookupTable, std::uint16_t redirectPort);
		~WinUdpCorrelator() override;

		bool CorrelateNetwork(const Packet& packet, const std::function<void(IPacketContext&)>& callback) override;
		bool CorrelateSocket(const PacketMetadata& metadata, const std::function<void(IPacketContext&)>& callback) override;

	private:
		std::unordered_multimap<FiveTuple, Packet, FiveTupleHash> m_pending;

		IProcessGuard& m_guard;
		AssociationTable& m_associations;

		std::optional<WinUdpLookupTable> m_lookupTable;

		std::uint16_t m_redirectPort;
	};
}