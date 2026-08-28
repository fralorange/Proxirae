#pragma once

#include <vector>

#include "interception/IPacketHandler.h"
#include "interception/PacketDispatchContext.h"

namespace Proxirae {
	class PacketDispatcher {
	public:
		void RegisterHandler(IPacketHandler& handler);

		void Dispatch(PacketDispatchContext ctx);

	private:
		std::vector<IPacketHandler*> m_handlers{};
	};
}