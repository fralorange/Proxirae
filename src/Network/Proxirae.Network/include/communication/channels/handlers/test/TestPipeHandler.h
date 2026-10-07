#pragma once

#include "communication/channels/handlers/IPipeMessageHandler.h"
#include "controllers/test/TestController.h"
#include "protection/IProtector.h"

namespace Proxirae {
	class TestPipeHandler : public IPipeMessageHandler {
	public:
		TestPipeHandler(TestController& controller, IProtector& protector);

		void Handle(const PipeMessage& msg) override;

	private:
		TestController& m_controller;
		IProtector& m_protector;
	};
}