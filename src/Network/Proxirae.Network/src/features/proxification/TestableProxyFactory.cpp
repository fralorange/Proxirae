#include "features/proxification/TestableProxyFactory.h"
#include "features/proxification/stream/TestableTcpSocks5Proxy.h"
#include "features/proxification/stream/TestableTcpHttpsProxy.h"

namespace Proxirae {
    TestableProxyFactory::TestableProxyFactory(IAsyncDriver& driver, IIoStreamAdapter& adapter, ILogger& logger)
        : m_driver(driver), m_adapter(adapter), m_logger(logger) {
    }

    std::unique_ptr<IStreamProxy> TestableProxyFactory::CreateTestableStream(
        const ProxyTestContract&contract,
        DiagnosticsCallback callback)
    {
        switch (contract.type) {
        case ProxyType::SOCKS5:
            return std::make_unique<TestableTcpSocks5Proxy>(
                std::move(callback),
                contract.address,
                contract.port,
                contract.username,
                contract.password,
                m_driver,
                m_adapter,
                m_logger
            );

        case ProxyType::HTTPS:
            return std::make_unique<TestableTcpHttpsProxy>(
                std::move(callback),
                contract.address,
                contract.port,
                contract.username,
                contract.password,
                m_driver,
                m_adapter,
                m_logger
            );
        }

        return nullptr;
    }
}