#pragma once

#include "communication/channels/handlers/IPipeMessageHandler.h"
#include "controllers/session/SessionController.h"

namespace Proxirae {
	class FlowPipeHandler : public IPipeMessageHandler {
	public:
		FlowPipeHandler(SessionController& controller);

		void Handle(const PipeMessage& msg) override;

	private:
		SessionController& m_controller;
	};
}