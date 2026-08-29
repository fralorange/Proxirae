#include <format>

#include "environment/sock.h"
#include "environment/inet.h"
#include "transport/TcpSession.h"
#include "utils/UuidUtils.h"

namespace Proxirae {
	class TcpSession::TcpBridge {
	public:
		NativeSocket client;
		Endpoint endpoint;

		IIoDriver& driver;
		ILogger& logger;
		IProxyFactory& proxyFactory;

		std::unique_ptr<IProxy> proxy;

		std::function<void(std::shared_ptr<TcpSession>)> onTerminated;
		std::atomic_bool isStopping{ false };

		std::vector<std::byte> clientBuffer;
		std::vector<std::byte> proxyBuffer;

		std::string proxyId;
		std::uint32_t dstAddress{ 0 };
		std::uint16_t dstPort{ 0 };
		std::int64_t processId{ 0 };

		std::atomic<std::uint64_t> bytesSent{ 0 };
		std::atomic<std::uint64_t> bytesReceived{ 0 };

		TcpBridge(NativeSocket client, Endpoint endpoint, IIoDriver& driver, ILogger& logger, IProxyFactory& factory)
			: client(client), endpoint(endpoint), driver(driver), logger(logger), proxyFactory(factory) {
			clientBuffer.resize(4096);
			proxyBuffer.resize(4096);
		}

		bool Open(std::shared_ptr<TcpSession> self, const FiveTuple& key, const ConnectionEntry& entry, std::function<void(std::shared_ptr<TcpSession>)> cb) {
			onTerminated = std::move(cb);

			if (!entry.proxyId.has_value()) {
				return false;
			}

			proxy = proxyFactory.Create(*entry.proxyId);

			char targetHost[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &key.dstAddress, targetHost, sizeof(targetHost));
			int targetPort = ntohs(key.dstPort);

			if (!proxy->Connect(targetHost, targetPort)) {
				return false;
			}

			proxyId = *entry.proxyId;
			dstAddress = key.dstAddress;
			dstPort = key.dstPort;
			processId = entry.processId.has_value() ? *entry.processId : -1;

			ForwardToProxy(self);
			ForwardToClient(self);

			return true;
		}

		void Close(std::shared_ptr<TcpSession> self) {
			bool expected = false;
			if (!isStopping.compare_exchange_strong(expected, true)) {
				return; 
			}

			if (client != InvalidNativeSocket) {
				shutdown(client, ShutdownBoth);
				CloseSocket(client);
				client = InvalidNativeSocket;
			}

			if (onTerminated && self) {
				onTerminated(self);
			}
		}

		void ForwardToProxy(std::shared_ptr<TcpSession> self) {
			if (isStopping) {
				return;
			}

			driver.AsyncRead(client, std::span(clientBuffer), [this, self](const IoResult& res) {
				if (!res.success || res.bytesTransferred == 0) {
					if (!isStopping) logger.LogDebug(std::format("[TcpSession] Client connection closed ({}:{})", endpoint.GetAddress(), endpoint.GetPort()));
					self->Terminate();

					return;
				}

				auto payload = std::span<const std::byte>(clientBuffer.data(), res.bytesTransferred);

				proxy->Send(payload, [this, self, bytes = res.bytesTransferred](const IoResult& wRes) {
					if (!wRes.success) {
						if (!isStopping) logger.LogError(std::format("[TcpSession] Failed to send data to proxy: error {}", wRes.errorCode));
						self->Terminate();

						return;
					}

					bytesSent.fetch_add(bytes, std::memory_order_relaxed);

					logger.LogDebug(std::format("[TcpSession] Forwarded {} bytes (Client -> Proxy)", bytes));

					ForwardToProxy(self);
				});
			});
		}

		void ForwardToClient(std::shared_ptr<TcpSession> self) {
			if (isStopping) return;

			proxy->Recv(std::span(proxyBuffer), [this, self](const IoResult& res) {
				if (!res.success || res.bytesTransferred == 0) {
					if (!isStopping) logger.LogDebug(std::format("[TcpSession] Proxy connection closed for {}", proxyId));
					self->Terminate();

					return;
				}

				auto payload = std::span<const std::byte>(proxyBuffer.data(), res.bytesTransferred);

				driver.AsyncWrite(client, payload, [this, self, bytes = res.bytesTransferred](const IoResult& wRes) {
					if (!wRes.success) {
						if (!isStopping) logger.LogError(std::format("[TcpSession] Failed to send data to client: error {}", wRes.errorCode));
						self->Terminate();

						return;
					}

					bytesReceived.fetch_add(bytes, std::memory_order_relaxed);

					logger.LogDebug(std::format("[TcpSession] Forwarded {} bytes (Proxy -> Client)", bytes));

					ForwardToClient(self);
				});
			});
		}
	};

	TcpSession::TcpSession(NativeSocket client, Endpoint endpoint, IIoDriver& driver, ILogger& logger, IProxyFactory& factory)
		: m_bridge(std::make_unique<TcpBridge>(client, endpoint, driver, logger, factory)) 
	{
		m_id = UuidUtils::GenerateUUID();
	}

	TcpSession::~TcpSession() 
	{
		Terminate();
	}

	std::string_view TcpSession::GetId() const
	{
		return m_id;
	}

	std::uint32_t TcpSession::GetAddress() const
	{
		return m_bridge->endpoint.GetAddress();
	}

	std::uint16_t TcpSession::GetPort() const
	{
		return m_bridge->endpoint.GetPort();
	}

	void TcpSession::Establish(const FiveTuple& key, const ConnectionEntry& entry, std::function<void(std::shared_ptr<TcpSession>)> onTerminated)
	{
		m_start = std::chrono::steady_clock::now();

		if (!m_bridge->Open(shared_from_this(), key, entry, std::move(onTerminated))) {
			Terminate();
		}
	}

	void TcpSession::Terminate() 
	{
		std::shared_ptr<TcpSession> self = nullptr;
		try {
			self = weak_from_this().lock();
		}
		catch (const std::bad_weak_ptr&) {}
		m_bridge->Close(self);
	}

	FlowContract TcpSession::GetFlow() const
	{
		auto now = std::chrono::steady_clock::now();
		auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now - m_start).count();

		return FlowContract{
			.id = m_id,
			.targetAddress = m_bridge->dstAddress,
			.targetPort = m_bridge->dstPort,
			.processId = m_bridge->processId,
			.secondsPassed = static_cast<std::uint64_t>(seconds),
			.proxyId = m_bridge->proxyId,
			.bytesSent = m_bridge->bytesSent.load(std::memory_order_relaxed),
			.bytesReceived = m_bridge->bytesReceived.load(std::memory_order_relaxed),
			.status = m_bridge->isStopping ? FlowStatus::Closing : FlowStatus::Active
		};
	}
}