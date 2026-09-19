namespace Proxirae {
	template <typename... Args>
	TestableTcpSocks5Proxy::TestableTcpSocks5Proxy(DiagnosticsCallback callback, Args&&... args)
		: TcpSocks5Proxy(std::forward<Args>(args)...), m_callback(std::move(callback)) { }
}