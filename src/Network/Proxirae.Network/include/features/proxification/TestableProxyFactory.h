#pragma once

#include <memory>

#include "features/proxification/stream/IStreamProxy.h"
#include "asyncio/async/IAsyncDriver.h"
#include "asyncio/io/stream/IIoStreamAdapter.h"
#include "features/diagnostics/ILogger.h"
#include "features/proxification/stream/DiagnosticsCallback.h"
#include "core/contracts/proxy/ProxyTestContract.h" 

namespace Proxirae {
    class TestableProxyFactory {
    public:
        TestableProxyFactory(IAsyncDriver& driver, IIoStreamAdapter& adapter, ILogger& logger);

        std::unique_ptr<IStreamProxy> CreateTestableStream(
            const ProxyTestContract& contract,
            DiagnosticsCallback callback
        );

    private:
        IAsyncDriver& m_driver;
        IIoStreamAdapter& m_adapter;
        ILogger& m_logger;
    };
}