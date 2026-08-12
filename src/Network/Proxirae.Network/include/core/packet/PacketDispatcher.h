#pragma once

#include <vector>

#include "core/packet/IPacketHandler.h"
#include "core/packet/PacketDispatchContext.h"

namespace Proxirae {
	class PacketDispatcher {
	public:
		void RegisterHandler(IPacketHandler& handler);

		void Dispatch(PacketDispatchContext ctx);

	private:
		std::vector<IPacketHandler*> m_handlers{};
	};
}