#pragma once

#include "features/communication/channels/messengers/IpcMessenger.h"
#include "features/diagnostics/ILogger.h"
#include "core/contracts/test/TestContract.h"
#include "features/proxification/TestableProxyFactory.h"

namespace Proxirae {
	class TestController {
	public:
		TestController(IpcMessenger& messenger, TestableProxyFactory& proxyFactory, ILogger& logger);

		void Test(TestContract& test);

	private:
		IpcMessenger& m_messenger;
		ILogger& m_logger;
		TestableProxyFactory& m_proxyFactory;
	};
}