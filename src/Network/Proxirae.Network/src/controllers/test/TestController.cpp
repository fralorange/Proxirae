#include "controllers/test/TestController.h"
#include "contracts/test/TestProgressContract.h"

namespace Proxirae {
    TestController::TestController(IpcMessenger& messenger, TestableProxyFactory& proxyFactory, ILogger& logger)
        : m_messenger(messenger), m_proxyFactory(proxyFactory), m_logger(logger) { }

    void TestController::Test(TestContract& test)
    {
        DiagnosticsCallback callback = [this, id = test.id](TestStage stage, std::uint64_t latency, bool success) {
            TestProgressContract progress{ .id = id };

            if (!success) {
                progress.stage = static_cast<std::uint8_t>(TestStage::Failed);
                progress.latency = std::nullopt;
            }
            else {
                progress.stage = static_cast<std::uint8_t>(stage);
                progress.latency = latency;
            }

            m_messenger.Send(PipeMessageType::Evt_SendTestProgress, progress);
            };

        auto proxy = m_proxyFactory.CreateTestableStream(test.proxy, std::move(callback));
        if (!proxy) {
            m_logger.LogError(std::format("[TestController] Failed to create proxy for test {}", test.id));
            return;
        }

        proxy->Connect(test.testAddress, test.testPort);
    }
}