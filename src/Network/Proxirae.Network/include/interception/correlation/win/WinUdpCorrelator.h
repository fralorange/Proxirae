#pragma once

#include <memory>

#include "interception/correlation/IPacketCorrelator.h"
#include "persistence/AssociationTable.h"
#include "interception/diversion/IPacketContext.h"
#include "interception/diversion/Packet.h"
#include "processes/IProcessGuard.h"

namespace Proxirae {
	class WinUdpCorrelator : public IPacketCorrelator {
	public:
		WinUdpCorrelator(IProcessGuard& guard, AssociationTable& associations);
		~WinUdpCorrelator() override;

		bool CorrelateNetwork(const Packet& packet, const std::function<void(IPacketContext&)>& callback) override;
		bool CorrelateSocket(const PacketMetadata& metadata, const std::function<void(IPacketContext&)>& callback) override;

	private:
		class PendingPacketCache;
		std::unique_ptr<PendingPacketCache> m_cache;

		IProcessGuard& m_guard;
		AssociationTable& m_associations;
	};
}