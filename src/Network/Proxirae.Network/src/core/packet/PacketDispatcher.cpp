#include "core/packet/PacketDispatcher.h"

namespace Proxirae {
	void PacketDispatcher::RegisterHandler(IPacketHandler& handler)
	{
		m_handlers.push_back(&handler);
	}

	void PacketDispatcher::Dispatch(PacketDispatchContext ctx)
	{
		for (auto* handler : m_handlers) {
			if (handler->CanHandle(ctx.packetCtx)) {
				handler->Handle(ctx);

				return;
			}
		}
	}
}