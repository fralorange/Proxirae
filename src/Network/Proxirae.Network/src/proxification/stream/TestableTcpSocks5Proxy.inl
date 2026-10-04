namespace Proxirae {
    template <typename... Args>
    TestableTcpSocks5Proxy::TestableTcpSocks5Proxy(DiagnosticsCallback callback, Args&&... args)
        : TcpSocks5Proxy(std::forward<Args>(args)...), TestableProxyBase(std::move(callback)) {
    }
}