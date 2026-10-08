#pragma once

#include "features/communication/channels/handlers/IPipeMessageHandler.h"
#include "features/communication/controllers/test/TestController.h"
#include "platform/protection/IProtector.h"

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