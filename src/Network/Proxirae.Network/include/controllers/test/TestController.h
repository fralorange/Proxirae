#pragma once

#include "communication/channels/messengers/IpcMessenger.h"
#include "diagnostics/ILogger.h"
#include "contracts/test/TestContract.h"
#include "proxification/TestableProxyFactory.h"

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