#pragma once

#include "features/communication/channels/handlers/IPipeMessageHandler.h"
#include "features/communication/controllers/session/SessionController.h"

namespace Proxirae {
	class FlowPipeHandler : public IPipeMessageHandler {
	public:
		FlowPipeHandler(SessionController& controller);

		void Handle(const PipeMessage& msg) override;

	private:
		SessionController& m_controller;
	};
}