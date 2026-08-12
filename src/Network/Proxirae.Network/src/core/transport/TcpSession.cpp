#include <format>

#include "platform/sock.h"
#include "platform/inet.h"
#include "core/transport/TcpSession.h"

namespace Proxirae {
	TcpSession::TcpSession(NativeSocket client, Endpoint endpoint, IIoDriver& driver, ILogger& logger, IProxyFactory& factory)
		: m_client(client), m_endpoint(endpoint), m_driver(driver), m_logger(logger), m_proxyFactory(factory)
	{ }

	TcpSession::~TcpSession() {
		Terminate();
	}

	std::uint32_t TcpSession::GetAddress() const
	{
		return m_endpoint.GetAddress();
	}

	std::uint16_t TcpSession::GetPort() const
	{
		return m_endpoint.GetPort();
	}

	void TcpSession::Handle(const FiveTuple& key, const ConnectionEntry& entry, std::function<void(std::shared_ptr<TcpSession>)> onTerminated)
	{
		m_onTerminated = std::move(onTerminated);

		if (!entry.proxyId.has_value()) {
			Terminate();
			return;
		}

		m_proxy = m_proxyFactory.Create(*entry.proxyId);

		char targetHost[INET_ADDRSTRLEN];
		int targetPort;

		inet_ntop(AF_INET, &key.dstAddress, targetHost, sizeof(targetHost));
		targetPort = ntohs(key.dstPort);

		if (!m_proxy->Connect(targetHost, targetPort)) {
			Terminate();
			return;
		}

		StartClientToProxy();
		StartProxyToClient();
	}

	void TcpSession::Terminate() {
		bool expected = false;
		if (!m_isStopping.compare_exchange_strong(expected, true)) {
			return;
		}

		if (m_client != InvalidNativeSocket) {
			shutdown(m_client, ShutdownBoth);
			CloseSocket(m_client);
			m_client = InvalidNativeSocket;
		}

		if (m_proxy) {
			m_proxy->Disconnect();
		}

		if (m_onTerminated) {
			m_onTerminated(shared_from_this());
		}
	}

	void TcpSession::StartClientToProxy()
	{
		if (m_isStopping) {
			return;
		}

		auto self = shared_from_this();

		m_driver.AsyncRead(m_client, std::span(m_clientBuffer), [this, self](const IoResult& res) {
			if (!res.success || res.bytesTransferred == 0) {
				if (!m_isStopping) m_logger.LogDebug("Terminated DAEMON connection.");
				Terminate();

				return;
			}

			m_logger.LogInfo(std::format("Received data from DAEMON: Length={}", res.bytesTransferred));

			auto payload = std::span<const char>(m_clientBuffer.data(), res.bytesTransferred);

			m_proxy->Send(payload, [this, self](const IoResult& wRes) {
				if (!wRes.success) {
					m_logger.LogError(std::format("Failed to send data to PROXY. Error={}", wRes.errorCode));
					Terminate();

					return;
				}

				m_logger.LogInfo("Sent data from DAEMON to PROXY");

				StartClientToProxy();
			});
		});
	}

	void TcpSession::StartProxyToClient()
	{
		if (m_isStopping) return;

		auto self = shared_from_this();

		m_proxy->Recv(std::span(m_proxyBuffer), [this, self](const IoResult& res) {
			if (!res.success || res.bytesTransferred == 0) {
				if (!m_isStopping) m_logger.LogDebug("Terminated PROXY connection.");
				Terminate();

				return;
			}

			m_logger.LogInfo(std::format("Received data from PROXY: Length={}", res.bytesTransferred));

			auto payload = std::span<const char>(m_proxyBuffer.data(), res.bytesTransferred);

			m_driver.AsyncWrite(m_client, payload, [this, self](const IoResult& wRes) {
				if (!wRes.success) {
					m_logger.LogError(std::format("Failed to send data to CLIENT. Error={}", wRes.errorCode));
					Terminate();

					return;
				}

				m_logger.LogInfo("Sent data from PROXY to CLIENT");

				StartProxyToClient();
			});
		});
	}
}