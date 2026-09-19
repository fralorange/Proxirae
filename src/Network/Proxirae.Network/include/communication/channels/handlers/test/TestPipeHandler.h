#pragma once

#include "communication/channels/handlers/IPipeMessageHandler.h"
#include "controllers/test/TestController.h"

namespace Proxirae {
	class TestPipeHandler : public IPipeMessageHandler {
	public:
		TestPipeHandler(TestController& controller);

		void Handle(const PipeMessage& msg) override;

	private:
		TestController& m_controller;
	};
}