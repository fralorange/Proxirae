#pragma once

#include <vector>

#include "core/packet/PacketDispatcher.h"
#include "core/packet/IPacketHandler.h"
#include "core/packet/IPacketContext.h"

namespace Proxirae {
	class PacketDispatcher {
	public:
		void RegisterHandler(IPacketHandler& handler);

		void Dispatch(IPacketContext& packet);

	private:
		std::vector<IPacketHandler*> m_handlers{};
	};
}