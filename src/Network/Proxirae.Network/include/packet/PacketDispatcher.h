#pragma once

#include <vector>

#include "packet/IPacketHandler.h"
#include "packet/PacketDispatchContext.h"

namespace Proxirae {
	class PacketDispatcher {
	public:
		void RegisterHandler(IPacketHandler& handler);

		void Dispatch(PacketDispatchContext ctx);

	private:
		std::vector<IPacketHandler*> m_handlers{};
	};
}