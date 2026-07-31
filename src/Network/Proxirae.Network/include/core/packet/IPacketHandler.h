#pragma once

#include "core/packet/IPacketContext.h"

namespace Proxirae {
	class IPacketHandler {
	public:
		virtual ~IPacketHandler() = default;

		virtual bool CanHandle(const IPacketContext& ctx) = 0;
		virtual void Handle(IPacketContext& ctx) = 0;
	};
}