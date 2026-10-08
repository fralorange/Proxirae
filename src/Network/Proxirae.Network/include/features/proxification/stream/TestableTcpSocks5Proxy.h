#pragma once

#include "features/proxification/stream/TcpSocks5Proxy.h"
#include "features/proxification/TestableProxyBase.h"

namespace Proxirae {
    class TestableTcpSocks5Proxy : public TcpSocks5Proxy, public TestableProxyBase {
    public:
        template <typename... Args>
        TestableTcpSocks5Proxy(DiagnosticsCallback callback, Args&&... args);

    protected:
        NativeSocket ConnectToProxy() override;
        bool PerformHandshake(NativeSocket sock) override;
        bool ConnectToTarget(NativeSocket sock, std::string_view targetAddress, std::uint16_t targetPort) override;
    };
}

#include "features/proxification/stream/TestableTcpSocks5Proxy.inl"