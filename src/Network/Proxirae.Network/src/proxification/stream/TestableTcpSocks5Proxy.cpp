#include "proxification/stream/TestableTcpSocks5Proxy.h"

namespace Proxirae {
	NativeSocket TestableTcpSocks5Proxy::ConnectToProxy()
	{
		auto start = std::chrono::steady_clock::now();
		auto sock = TcpSocks5Proxy::ConnectToProxy();
		auto end = std::chrono::steady_clock::now();

		auto result = sock != InvalidNativeSocket;

		Report(TestStage::Establish, start, end, result);

		return sock;
	}

	bool TestableTcpSocks5Proxy::PerformHandshake(NativeSocket sock)
	{
		auto start = std::chrono::steady_clock::now();
		auto result = TcpSocks5Proxy::PerformHandshake(sock);
		auto end = std::chrono::steady_clock::now();

		Report(TestStage::Handshake, start, end, result);

		return result;
	}

	bool TestableTcpSocks5Proxy::ConnectToTarget(NativeSocket sock, std::string_view targetAddress, std::uint16_t targetPort)
	{
		auto start = std::chrono::steady_clock::now();
		auto result = TcpSocks5Proxy::ConnectToTarget(sock, targetAddress, targetPort);
		auto end = std::chrono::steady_clock::now();

		Report(TestStage::Connect, start, end, result);

		return result;
	}

	void TestableTcpSocks5Proxy::Report(TestStage stage, std::chrono::steady_clock::time_point start, std::chrono::steady_clock::time_point end, bool success)
	{
		if (m_callback) {
			auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
			m_callback(stage, ms, success);
		}
	}
}