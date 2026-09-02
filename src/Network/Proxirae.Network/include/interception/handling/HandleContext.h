#pragma once
#include <optional>
#include <string>

#include "interception/diversion/IPacketContext.h"

namespace Proxirae {
	struct HandleContext {
		IPacketContext& packetCtx;
		std::optional<std::string> proxyId;
	};
}