#include "interception/handling/DispatcherHandler.h"

namespace Proxirae {
	void DispatcherHandler::RegisterHandler(IPacketHandler& handler)
	{
		m_handlers.push_back(&handler);
	}

	bool DispatcherHandler::Handle(HandleContext& ctx)
	{
		for (auto* handler : m_handlers) {
			if (handler->Handle(ctx)) {
				return true;
			}
		}

		return false;
	}
}