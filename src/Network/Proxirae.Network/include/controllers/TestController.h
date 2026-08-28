#pragma once

#include "ipc/IpcMessenger.h"
#include "io/IIoDriver.h"
#include "diagnostics/ILogger.h"
#include "contracts/test/TestContract.h"

namespace Proxirae {
	class TestController {
	public:
		TestController(IpcMessenger& messenger, IIoDriver& driver, ILogger& logger);

		void Test(TestContract& test);

	private:
		IpcMessenger& m_messenger;
		IIoDriver& m_driver;
		ILogger& m_logger;
	};
}