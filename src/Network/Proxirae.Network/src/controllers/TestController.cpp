#include "controllers/TestController.h"
#include "proxification/TestableSocks5Proxy.h"
#include "contracts/test/TestProgressContract.h"

namespace Proxirae {
	TestController::TestController(IpcMessenger& messenger, IIoDriver& driver, ILogger& logger)
		: m_messenger(messenger), m_driver(driver), m_logger(logger) { }

	void TestController::Test(TestContract& test)
	{
		DiagnosticsCallback callback = [this, &test](ProxyTestStage stage, std::uint64_t latency, bool success) {
			TestProgressContract progress{ .id = test.id };

			if (!success) {
				progress.stage = static_cast<std::uint8_t>(ProxyTestStage::Failed);
				progress.latency = std::nullopt;
			}
			else {
				progress.stage = static_cast<std::uint8_t>(stage);
				progress.latency = latency;
			}

			m_messenger.Send(PipeMessageType::Evt_SendTestProgress, progress);
		};

		TestableSocks5Proxy testableProxy(
			callback,
			test.proxy.address,
			test.proxy.port,
			test.proxy.username,
			test.proxy.password,
			m_driver,
			m_logger
		);

		testableProxy.Connect(test.testAddress, test.testPort);
	}
}