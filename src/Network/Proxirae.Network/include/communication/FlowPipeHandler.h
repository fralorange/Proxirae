#pragma once

#include "communication/IPipeMessageHandler.h"
#include "controllers/SessionController.h"

namespace Proxirae {
	class FlowPipeHandler : public IPipeMessageHandler {
	public:
		FlowPipeHandler(SessionController& controller);

		void Handle(const PipeMessage& msg) override;

	private:
		SessionController& m_controller;
	};
}