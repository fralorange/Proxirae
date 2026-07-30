#pragma once

#include <vector>
#include <memory>

#include "IPacketHandler.h"

namespace Proxirae {
	class PacketDispatcher {
	public:
		void RegisterHandler(IPacketHandler& handler);

		void Dispatch(Packet& packet);

	private:
		std::vector<IPacketHandler*> m_handlers{};
	};
}