#include "transport/UdpMultiplexer.h"
#include "environment/sock.h"

// TODO: Refactor. Add a dedicated UdpClassificator (or similar) that creates/updates
// UDP connection states in ConnectionTable based on elapsed timeouts, on a background thread.
namespace Proxirae {
	class UdpMultiplexer::UdpProcessor {
	public:
		NativeSocket shared;
		IIoDatagramAdapter& adapter;
		ConnectionTable& connections;
		IFlowMonitor& monitor;
		IProxyFactory& proxyFactory;
		ILogger& logger;

		std::atomic<bool> isRunning{ false };
		std::thread collectThread;

		std::mutex sessionsMtx;
		std::vector<std::shared_ptr<UdpSession>> sessions;
		std::vector<std::byte> recvBuffer;

		UdpProcessor(NativeSocket socket, IIoDatagramAdapter& adapter, ConnectionTable& connections, IFlowMonitor& monitor, IProxyFactory& factory, ILogger& logger)
			: shared(socket), adapter(adapter), connections(connections), monitor(monitor), proxyFactory(factory), logger(logger)
		{
			recvBuffer.resize(65536);
		}

		~UdpProcessor() {
			Stop();
		}

		bool Start() {
			if (isRunning.exchange(true)) {
				return true;
			}

			StartAsyncReceive();
			collectThread = std::thread([this]() { CollectLoop(); });

			return true;
		}

		bool Stop() {
			if (!isRunning.exchange(false)) {
				return false;
			}

			if (shared != InvalidNativeSocket) {
				CloseSocket(shared); 
			}

			if (collectThread.joinable()) {
				collectThread.join();
			}

			std::vector<std::shared_ptr<UdpSession>> sessionsToTerminate;
			{
				std::lock_guard<std::mutex> lock(sessionsMtx);
				sessionsToTerminate = std::move(sessions);
			}

			for (auto& session : sessionsToTerminate) {
				if (session) {
					session->Terminate();
				}
			}

			return true;
		}

		void StartAsyncReceive() {
			if (!isRunning || shared == InvalidNativeSocket) {
				return;
			}

			adapter.AsyncRecvFrom(shared, std::span(recvBuffer), [this](const IoDatagramResult& res) {
				if (!isRunning) {
					return;
				}

				if (res.success && res.bytesTransferred > 0) {
					auto payload = std::span<const std::byte>(recvBuffer.data(), res.bytesTransferred);
					
					if (res.remoteAddr.ss_family == AF_INET) {
						const auto* addrIn = reinterpret_cast<const sockaddr_in*>(&res.remoteAddr);

						Endpoint clientEndpoint(addrIn->sin_addr.s_addr, addrIn->sin_port);

						ProcessPacket(payload, clientEndpoint);
					}
					else {
						// IPv6 ...
					}
				}

				StartAsyncReceive();
			});
		}

		void ProcessPacket(std::span<const std::byte> payload, const Endpoint& endpoint) {
			std::shared_ptr<UdpSession> session;
			{
				std::lock_guard<std::mutex> lock(sessionsMtx);
				auto it = std::find_if(sessions.begin(), sessions.end(),
					[&](const auto& s) { return s->GetAddress() == endpoint.GetAddress() && s->GetPort() == endpoint.GetPort(); });

				if (it != sessions.end()) {
					session = *it;
				}
			}

			if (!session) {
				session = std::make_shared<UdpSession>(shared, endpoint, adapter, proxyFactory, logger);

				ThreeTuple key{
					.srcAddress = session->GetAddress(),
					.srcPort = session->GetPort(),
					.protocol = IPPROTO_UDP
				};

				auto optKey = connections.FindKey(key);
				if (!optKey.has_value()) {
					session->Terminate();
					return;
				}

				auto optEntry = connections.GetConnection(*optKey);
				if (!optEntry.has_value()) {
					session->Terminate();
					return;
				}

				session->Establish(*optKey, *optEntry);

				std::lock_guard<std::mutex> lock(sessionsMtx);
				sessions.push_back(session);
			}

			session->OnData(payload);
		}

		void CollectLoop() {
			while (isRunning) {
				std::this_thread::sleep_for(std::chrono::seconds(5));

				std::vector<std::shared_ptr<UdpSession>> expired;
				{
					std::lock_guard<std::mutex> lock(sessionsMtx);
					for (auto it = sessions.begin(); it != sessions.end();) {
						if ((*it)->IsExpired(std::chrono::seconds(60))) {
							expired.push_back(*it);
							it = sessions.erase(it);
						}
						else {
							++it;
						}
					}
				}

				for (auto& session : expired) {
					monitor.ReportFlowClosed(session->GetFlow());

					ThreeTuple key{
							.srcAddress = session->GetAddress(),
							.srcPort = session->GetPort(),
							.protocol = IPPROTO_UDP
					};

					auto optKey = connections.FindKey(key);
					if (optKey.has_value()) {
						connections.RemoveConnection(*optKey);
					}
				}
			}
		}

		std::optional<std::vector<FlowContract>> GetActiveFlows() {
			std::lock_guard<std::mutex> lock(sessionsMtx);
			if (sessions.empty()) {
				return std::nullopt;
			}

			std::vector<FlowContract> snapshot;
			snapshot.reserve(sessions.size());

			for (const auto& session : sessions) {
				if (session) {
					snapshot.push_back(session->GetFlow());
				}
			}

			return snapshot;
		}

		void TerminateFlow(const std::string& id) {
			std::shared_ptr<UdpSession> target;
			{
				std::lock_guard<std::mutex> lock(sessionsMtx);
				for (const auto& session : sessions) {
					if (session && session->GetId() == id) {
						target = session;
						break;
					}
				}
			}

			if (target) {
				target->Terminate();
			}
		}
	};


	UdpMultiplexer::UdpMultiplexer(UdpBinder& binder, IIoDatagramAdapter& adapter, ConnectionTable& connections, IFlowMonitor& monitor, IProxyFactory& factory, ILogger& logger)
		: m_processor(std::make_unique<UdpProcessor>(binder.ReleaseSocket(), adapter, connections, monitor, factory, logger)) {
	}

	UdpMultiplexer::~UdpMultiplexer() = default;

	bool UdpMultiplexer::Start() {
		return m_processor->Start();
	}

	bool UdpMultiplexer::Stop() {
		return m_processor->Stop();
	}

	std::optional<std::vector<FlowContract>> UdpMultiplexer::GetActiveFlows() {
		return m_processor->GetActiveFlows();
	}

	void UdpMultiplexer::TerminateFlow(const std::string& id) {
		m_processor->TerminateFlow(id);
	}
}