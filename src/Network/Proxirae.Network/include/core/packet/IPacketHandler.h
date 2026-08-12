#pragma once

#include "core/packet/PacketDispatchContext.h"

namespace Proxirae {
	class IPacketHandler {
	public:
		virtual ~IPacketHandler() = default;

		virtual bool CanHandle(const IPacketContext& ctx) = 0;
		virtual void Handle(PacketDispatchContext& ctx) = 0;
	};
}