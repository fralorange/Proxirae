#pragma once

#include "ipc/IPipeMessageHandler.h"
#include "controllers/TestController.h"

namespace Proxirae {
	class TestPipeHandler : public IPipeMessageHandler {
	public:
		TestPipeHandler(TestController& controller);

		void Handle(const PipeMessage& msg) override;

	private:
		TestController& m_controller;
	};
}