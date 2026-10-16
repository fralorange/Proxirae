#pragma once

#include "features/communication/channels/messages/handlers/SafePipeMessageHandler.h"
#include "features/communication/controllers/test/TestController.h"
#include "platform/protection/IProtector.h"

namespace Proxirae {
	class TestPipeHandler : public SafePipeMessageHandler {
	public:
		TestPipeHandler(TestController& controller, IProtector& protector, ILogger& logger);

	protected:
		void HandleUnsafe(const PipeMessage& msg) override;

	private:
		TestController& m_controller;
		IProtector& m_protector;
	};
}