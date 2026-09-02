#pragma once

#include <vector>

#include "interception/handling/IPacketHandler.h"
#include "interception/handling/HandleContext.h"

namespace Proxirae {
	class DispatcherHandler : public IPacketHandler {
	public:
		void RegisterHandler(IPacketHandler& handler);
		
		bool Handle(HandleContext& ctx) override;

	private:
		std::vector<IPacketHandler*> m_handlers{};
	};
}