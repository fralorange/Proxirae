namespace Proxirae {
	template <typename... Args>
	TestableSocks5Proxy::TestableSocks5Proxy(DiagnosticsCallback callback, Args&&... args)
		: Socks5Proxy(std::forward<Args>(args)...), m_callback(std::move(callback)) { }
}