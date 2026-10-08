#pragma once

#include "features/proxification/stream/TcpHttpsProxy.h"
#include "features/proxification/TestableProxyBase.h"

namespace Proxirae {
    class TestableTcpHttpsProxy : public TcpHttpsProxy, public TestableProxyBase {
    public:
        template <typename... Args>
        TestableTcpHttpsProxy(DiagnosticsCallback callback, Args&&... args);

    protected:
        NativeSocket ConnectToProxy() override;
        bool ConnectToTarget(NativeSocket sock, std::string_view targetAddress, std::uint16_t targetPort) override;
    };
}

#include "features/proxification/stream/TestableTcpHttpsProxy.inl"