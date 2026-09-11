#pragma once

#include <functional>
#include <cstdint>
#include <chrono>

#include "proxification/stream/TcpSocks5Proxy.h"
#include "proxification/stream/TestStage.h"

namespace Proxirae {
	using DiagnosticsCallback = std::function<void(TestStage stage, std::uint64_t latencyMs, bool success)>;

	class TestableTcpSocks5Proxy : public TcpSocks5Proxy {
	public:
		
		template <typename... Args>
		TestableTcpSocks5Proxy(DiagnosticsCallback callback, Args&&... args);

	protected:
		NativeSocket ConnectToProxy() override;
		bool PerformHandshake(NativeSocket sock) override;
		bool ConnectToTarget(NativeSocket sock, std::string_view targetAddress, std::uint16_t targetPort) override;

	private:
		DiagnosticsCallback m_callback;

		void Report(TestStage stage, std::chrono::steady_clock::time_point start, std::chrono::steady_clock::time_point end, bool success);
	};
}

#include "proxification/stream/TestableTcpSocks5Proxy.inl"