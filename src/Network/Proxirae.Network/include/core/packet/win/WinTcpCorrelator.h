#pragma once

#include <unordered_map>

#include "core/registry/AssociationTable.h"
#include "core/registry/AssociationKey.h"
#include "core/registry/AssociationKeyHash.h"
#include "core/packet/IPacketContext.h"
#include "core/packet/Packet.h"

namespace Proxirae {
	class WinTcpCorrelator {
	public:
		WinTcpCorrelator(AssociationTable& associations);

		void CorrelateNetwork(const Packet& packet, const std::function<void(IPacketContext&)>& callback);
		void CorrelateSocket(const PacketMetadata& metadata, const std::function<void(IPacketContext&)>& callback);

	private:
		std::unordered_multimap<AssociationKey, Packet, AssociationKeyHash> m_pending;

		AssociationTable& m_associations;
	};
}