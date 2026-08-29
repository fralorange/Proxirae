#pragma once
#include <optional>
#include <string>

#include "interception/IPacketContext.h"

namespace Proxirae {
	struct PacketDispatchContext {
		IPacketContext& packetCtx;
		std::optional<std::string> proxyId;
	};
}