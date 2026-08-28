#pragma once

#include <functional>
#include <cstdint>
#include <chrono>

#include "proxy/Socks5Proxy.h"
#include "proxy/ProxyTestStage.h"

namespace Proxirae {
	using DiagnosticsCallback = std::function<void(ProxyTestStage stage, std::uint64_t latencyMs, bool success)>;

	class TestableSocks5Proxy : public Socks5Proxy {
	public:
		
		template <typename... Args>
		TestableSocks5Proxy(DiagnosticsCallback callback, Args&&... args);

	protected:
		NativeSocket ConnectToProxy() override;
		bool PerformHandshake(NativeSocket sock) override;
		bool ConnectToTarget(NativeSocket sock, std::string_view targetAddress, std::uint16_t targetPort) override;

	private:
		DiagnosticsCallback m_callback;

		void Report(ProxyTestStage stage, std::chrono::steady_clock::time_point start, std::chrono::steady_clock::time_point end, bool success);
	};
}

#include "proxy/TestableSocks5Proxy.inl"