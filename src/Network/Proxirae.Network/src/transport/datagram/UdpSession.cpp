#include <vector>

#include "transport/datagram/UdpSession.h"
#include "environment/sock.h"
#include "utils/UuidUtils.h"
#include "utils/SocketUtils.h"

namespace Proxirae {
	class UdpSession::UdpBridge {
	public:
		NativeSocket shared;
		Endpoint endpoint;

		IIoDatagramAdapter& adapter;
		ILogger& logger;
		IProxyFactory& proxyFactory;

		std::unique_ptr<IDatagramProxy> proxy;
		std::vector<std::byte> proxyBuffer;

		std::atomic<bool> isStopping{ false };

		std::string targetAddress;
		std::uint16_t targetPort{ 0 };

		std::string proxyId;
		std::int64_t processId{ -1 };

		std::atomic<std::uint64_t> bytesSent{ 0 };
		std::atomic<std::uint64_t> bytesReceived{ 0 };

		std::atomic<std::uint64_t> lastSeen;

		UdpBridge(NativeSocket shared, Endpoint endpoint, IIoDatagramAdapter& adapter, ILogger& logger, IProxyFactory& factory)
			: shared(shared), endpoint(endpoint), adapter(adapter), logger(logger), proxyFactory(factory) {
			proxyBuffer.resize(65536);
			UpdateLastSeen();
		}

		void UpdateLastSeen() {
			auto now = std::chrono::steady_clock::now().time_since_epoch();
			lastSeen.store(std::chrono::duration_cast<std::chrono::milliseconds>(now).count(), std::memory_order_relaxed);
		}

		std::chrono::steady_clock::time_point GetLastSeen() const {
			auto ms = lastSeen.load(std::memory_order_relaxed);
			return std::chrono::steady_clock::time_point(std::chrono::milliseconds(ms));
		}

		bool Open(std::shared_ptr<UdpSession> self, const FiveTuple& key, const ConnectionEntry& entry) {
			if (!entry.proxyId.has_value()) {
				return false;
			}

			proxy = proxyFactory.CreateDatagram(*entry.proxyId);
			if (!proxy) {
				return false;
			}

			targetAddress = key.dstAddress.ToString();
			targetPort = key.dstPort;

			if (!proxy->Connect()) {
				return false;
			}

			proxyId = *entry.proxyId;
			processId = entry.processId.has_value() ? *entry.processId : -1;

			ForwardToClient(self);

			return true;
		}

		void Close() {
			bool expected = false;
			if (!isStopping.compare_exchange_strong(expected, true)) {
				return;
			}

			if (proxy) {
				proxy->Disconnect();
			}
		}

		void ForwardToProxy(std::shared_ptr<UdpSession> self, std::span<const std::byte> payload) {
			if (isStopping || !proxy) {
				return;
			}

			UpdateLastSeen();

			proxy->Send(payload, targetAddress, targetPort, [this, self](const IoResult& res) {
				if (!res.success || res.bytesTransferred == 0) {
					if (!isStopping) 
						logger.LogDebug(std::format("[UdpSession] Client connection closed ({}:{})", endpoint.GetAddress().ToString(), endpoint.GetPort()));
					self->Terminate();

					return;
				}

				bytesSent.fetch_add(res.bytesTransferred, std::memory_order_relaxed);
			});
		}

		void ForwardToClient(std::shared_ptr<UdpSession> self) {
			if (isStopping || !proxy) {
				return;
			}

			proxy->Recv(std::span(proxyBuffer), [this, self](const IoResult& res, std::string sourceAddress, std::uint16_t sourcePort) {
				if (!res.success || res.bytesTransferred == 0) {
					if (!isStopping) logger.LogDebug(std::format("[UdpSession] Proxy connection closed for {}", proxyId));
					self->Terminate();

					return;
				}

				UpdateLastSeen();
				bytesReceived.fetch_add(res.bytesTransferred, std::memory_order_relaxed);

				sockaddr_storage clientAddrStorage{};
				int addrLen = SocketUtils::ToSockAddr(endpoint, clientAddrStorage);

				auto payload = std::make_shared<std::vector<std::byte>>(
					proxyBuffer.data(),
					proxyBuffer.data() + res.bytesTransferred
				);

				adapter.AsyncSendTo(
					shared, 
					reinterpret_cast<const sockaddr*>(&clientAddrStorage),
					addrLen,
					*payload, 
					[this, self, bytes = res.bytesTransferred](const auto& wRes) {
					
					if (!wRes.success) {
						if (!isStopping) logger.LogError(std::format("[UdpSession] Failed to send data to client"));
						self->Terminate();
						return;
					}
				});

				ForwardToClient(self);
			});
		}
	};

	UdpSession::UdpSession(NativeSocket shared, Endpoint endpoint, IIoDatagramAdapter& adapter, IProxyFactory& factory, ILogger& logger)
		: m_bridge(std::make_unique<UdpBridge>(shared, endpoint, adapter, logger, factory))
	{ 
		m_id = UuidUtils::GenerateUUID();
	}

	UdpSession::~UdpSession()
	{
		Terminate();
	}

	bool UdpSession::Establish(const FiveTuple& key, const ConnectionEntry& entry)
	{
		m_start = std::chrono::steady_clock::now();

		if (!m_bridge->Open(shared_from_this(), key, entry)) {
			Terminate();

			return false;
		}

		return true;
	}

	void UdpSession::Terminate()
	{
		m_bridge->Close();
	}

	bool UdpSession::IsExpired(std::chrono::seconds baseTimeout) const
	{
		if (m_bridge->isStopping) {
			return true;
		}

		std::uint16_t destPort = m_bridge->targetPort;

		std::chrono::seconds actualTimeout = baseTimeout;

		if (destPort == 53 || destPort == 123) {
			actualTimeout = std::chrono::seconds(15);
		}
		else if (destPort >= 10000) {
			actualTimeout = std::chrono::minutes(5);
		}

		return m_bridge->GetLastSeen() + actualTimeout < std::chrono::steady_clock::now();
	}

	void UdpSession::OnData(std::span<const std::byte> payload)
	{
		m_bridge->ForwardToProxy(shared_from_this(), payload);
	}

	FlowContract UdpSession::GetFlow() const
	{
		auto now = std::chrono::steady_clock::now();
		auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now - m_start).count();

		return FlowContract{
			.id = m_id,
			.targetAddress = m_bridge->targetAddress,
			.targetPort = m_bridge->targetPort,
			.processId = m_bridge->processId,
			.secondsPassed = static_cast<std::uint64_t>(seconds),
			.proxyId = m_bridge->proxyId,
			.bytesSent = m_bridge->bytesSent.load(std::memory_order_relaxed),
			.bytesReceived = m_bridge->bytesReceived.load(std::memory_order_relaxed),
			.status = m_bridge->isStopping ? FlowStatus::Closing : FlowStatus::Active
		};
	}

	std::string_view UdpSession::GetId() const
	{
		return m_id;
	}

	IpAddress UdpSession::GetAddress() const
	{
		return m_bridge->endpoint.GetAddress();
	}

	std::uint16_t UdpSession::GetPort() const
	{
		return m_bridge->endpoint.GetPort();
	}
}