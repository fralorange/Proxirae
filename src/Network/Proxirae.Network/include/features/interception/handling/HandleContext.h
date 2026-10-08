#pragma once
#include <optional>
#include <string>

#include "features/interception/diversion/IPacketContext.h"

namespace Proxirae {
	struct HandleContext {
		IPacketContext& packetCtx;
		std::optional<std::string> proxyId;
	};
}