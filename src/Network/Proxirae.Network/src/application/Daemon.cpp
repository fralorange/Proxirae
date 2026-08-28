#include <string>
#include <format>

#include "application/Daemon.h"
#include "platform/inet.h"
#include "persistence/ThreeTuple.h"

namespace Proxirae {
	Daemon::Daemon(TcpListener& listener, ConnectionTable& connections, ILogger& logger, IFlowMonitor& monitor, std::stop_token token)
		: m_listener(listener), 
		  m_connections(connections), 
		  m_logger(logger),
		  m_monitor(monitor),
		  m_token(token) { }

	Daemon::~Daemon() {
		std::vector<std::shared_ptr<TcpSession>> sessionsToTerminate;
		{
			std::lock_guard<std::mutex> lock(m_sessions_mtx);
			sessionsToTerminate = std::move(m_sessions);
		}

		for (auto& session : sessionsToTerminate) {
			if (session) {
				session->Terminate();
			}
		}
	}

	void Daemon::Run(std::uint16_t port, std::function<void(bool)> onReady) {
		bool success = m_listener.Listen(port);

		if (!success) {
			m_logger.LogError(std::format("[Daemon] Failed to start: TCP listener closed on port {}", port));
			
			return;
		}

		onReady(success);

		std::stop_callback interruptCallback(m_token, [this]() {
			m_listener.Close();
		});


		while (!m_token.stop_requested()) {
			auto session = m_listener.Accept();

			if (!session) {
				continue;
			}

			ThreeTuple key{
				.srcAddress = session->GetAddress(),
				.srcPort = session->GetPort(),
				.protocol = IPPROTO_TCP
			};

			auto optKey = m_connections.FindKey(key);

			if (!optKey.has_value()) {
				session->Terminate();
				continue;
			}

			auto entryIt = m_connections.GetConnection(*optKey);

			if (!entryIt.has_value()) {
				session->Terminate();
				continue;
			}

			{
				std::lock_guard<std::mutex> lock(m_sessions_mtx);
				m_sessions.push_back(session);
			}

			session->Establish(optKey.value(), entryIt.value(), [this](auto s) {
				m_monitor.ReportFlowClosed(s->GetFlow());

				std::lock_guard<std::mutex> lock(m_sessions_mtx);
				auto it = std::find(m_sessions.begin(), m_sessions.end(), s);
				if (it != m_sessions.end()) {
					m_sessions.erase(it);
				}

				ThreeTuple key{
					.srcAddress = s->GetAddress(),
					.srcPort = s->GetPort(),
					.protocol = IPPROTO_TCP
				};

				auto optKey = m_connections.FindKey(key);

				if (optKey.has_value()) {
					m_connections.RemoveConnection(*optKey);
				}
			});
		}
	}

	std::optional<std::vector<FlowContract>> Daemon::GetActiveFlows()
	{
		std::vector<FlowContract> snapshot;
		{
			std::lock_guard<std::mutex> lock(m_sessions_mtx);
			if (m_sessions.empty()) {
				return std::nullopt;
			}

			snapshot.reserve(m_sessions.size());
			for (auto& session : m_sessions) {
				if (session) 
				{
					snapshot.push_back(session->GetFlow());
				}
			}
		}

		return snapshot;
	}

	void Daemon::TerminateFlow(std::string id)
	{
		std::shared_ptr<TcpSession> target;
		{
			std::lock_guard<std::mutex> lock(m_sessions_mtx);

			if (m_sessions.empty()) {
				return;
			}

			for (auto& session : m_sessions) {
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
}