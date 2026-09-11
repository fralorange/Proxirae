#pragma once

#include <functional>

#include "interception/diversion/IPacketContext.h"
#include "interception/diversion/Packet.h"

namespace Proxirae {
	class IPacketCorrelator {
	public:
		virtual ~IPacketCorrelator() = default;

		virtual bool CorrelateNetwork(const Packet& packet, const std::function<void(IPacketContext&)>& callback) = 0;
		virtual bool CorrelateSocket(const PacketMetadata& metadata, const std::function<void(IPacketContext&)>& callback) = 0;
	};
}