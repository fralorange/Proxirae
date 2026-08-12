#pragma once
#include <optional>
#include <string>

#include "core/packet/IPacketContext.h"

namespace Proxirae {
	struct PacketDispatchContext {
		IPacketContext& packetCtx;
		std::optional<std::string> proxyId;
	};
}