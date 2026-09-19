#pragma once

#include "communication/channels/messengers/IpcMessenger.h"
#include "asyncio/async/IAsyncDriver.h"
#include "asyncio/io/stream/IIoStreamAdapter.h"
#include "diagnostics/ILogger.h"
#include "contracts/test/TestContract.h"

namespace Proxirae {
	class TestController {
	public:
		TestController(IpcMessenger& messenger, IAsyncDriver& driver, IIoStreamAdapter& adapter, ILogger& logger);

		void Test(TestContract& test);

	private:
		IpcMessenger& m_messenger;
		IAsyncDriver& m_driver;
		IIoStreamAdapter& m_adapter;
		ILogger& m_logger;
	};
}