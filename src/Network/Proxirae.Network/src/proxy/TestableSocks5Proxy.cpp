#include "proxy/TestableSocks5Proxy.h"

namespace Proxirae {
	NativeSocket TestableSocks5Proxy::ConnectToProxy()
	{
		auto start = std::chrono::steady_clock::now();
		auto sock = Socks5Proxy::ConnectToProxy();
		auto end = std::chrono::steady_clock::now();

		auto result = sock != InvalidNativeSocket;

		Report(ProxyTestStage::Establish, start, end, result);

		return sock;
	}

	bool TestableSocks5Proxy::PerformHandshake(NativeSocket sock)
	{
		auto start = std::chrono::steady_clock::now();
		auto result = Socks5Proxy::PerformHandshake(sock);
		auto end = std::chrono::steady_clock::now();

		Report(ProxyTestStage::Handshake, start, end, result);

		return result;
	}

	bool TestableSocks5Proxy::ConnectToTarget(NativeSocket sock, std::string_view targetAddress, std::uint16_t targetPort)
	{
		auto start = std::chrono::steady_clock::now();
		auto result = Socks5Proxy::ConnectToTarget(sock, targetAddress, targetPort);
		auto end = std::chrono::steady_clock::now();

		Report(ProxyTestStage::Connect, start, end, result);

		return result;
	}

	void TestableSocks5Proxy::Report(ProxyTestStage stage, std::chrono::steady_clock::time_point start, std::chrono::steady_clock::time_point end, bool success)
	{
		if (m_callback) {
			auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
			m_callback(stage, ms, success);
		}
	}
}