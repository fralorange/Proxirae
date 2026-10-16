#pragma once

#include "features/communication/channels/messages/handlers/SafePipeMessageHandler.h"
#include "features/communication/controllers/session/SessionController.h"

namespace Proxirae {
	class FlowPipeHandler : public SafePipeMessageHandler {
	public:
		FlowPipeHandler(SessionController& controller, ILogger& logger);

	protected:
		void HandleUnsafe(const PipeMessage& msg) override;

	private:
		SessionController& m_controller;
	};
}