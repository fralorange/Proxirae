#pragma once

#include "interception/handling/HandleContext.h"

namespace Proxirae {
	class IPacketHandler {
	public:
		virtual ~IPacketHandler() = default;

		virtual bool Handle(HandleContext& ctx) = 0;
	};
}