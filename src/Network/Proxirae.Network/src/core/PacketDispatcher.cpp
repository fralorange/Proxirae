#include "core/PacketDispatcher.h"

namespace Proxirae {
	void PacketDispatcher::RegisterHandler(IPacketHandler& handler)
	{
		m_handlers.push_back(&handler);
	}

	void PacketDispatcher::Dispatch(Packet& packet)
	{
		for (auto* handler : m_handlers) {
			if (handler->CanHandle(packet)) {
				handler->Handle(packet);

				return;
			}
		}
	}
}