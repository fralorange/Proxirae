#pragma once

#include <utility>

namespace Proxirae {
    template <typename... Args>
    TestableTcpHttpsProxy::TestableTcpHttpsProxy(DiagnosticsCallback callback, Args&&... args)
        : TcpHttpsProxy(std::forward<Args>(args)...)
        , TestableProxyBase(std::move(callback)) { 
    }
}