#include <algorithm>
#include <format>

#include "transport/stream/TcpMultiplexer.h"
#include "primitives/tuples/ThreeTuple.h"
#include "environment/inet.h"

namespace Proxirae {
	TcpMultiplexer::TcpMultiplexer(TcpListener& listener, ConnectionTable& connections, IFlowMonitor& monitor, ILogger& logger, std::uint16_t port) 
		: m_listener(listener),
		  m_connections(connections),
		  m_monitor(monitor),
		  m_logger(logger),
		  m_port(port) { }

	TcpMultiplexer::~TcpMultiplexer() {
		Stop();
	}

	bool TcpMultiplexer::Start() {
		if (m_running.exchange(true)) {
			return true;
		}

		if (!m_listener.Listen(m_port)) {
			m_running = false;
			m_logger.LogError(std::format("[TcpMultiplexer] Failed to start TCP listener on port {}", m_port));
			return false;
		}

		m_acceptThread = std::thread([this]() { AcceptLoop(); });
		return true;
	}

	bool TcpMultiplexer::Stop() {
		if (!m_running.exchange(false)) {
			return false;
		}

		m_listener.Close();

		if (m_acceptThread.joinable()) {
			m_acceptThread.join();
		}

		std::vector<std::shared_ptr<TcpSession>> sessionsToTerminate;
		{
			std::lock_guard<std::mutex> lock(m_sessionsMtx);
			sessionsToTerminate = std::move(m_sessions);
		}

		for (auto& session : sessionsToTerminate) {
			if (session) {
				session->Terminate();
			}
		}

		return true;
	}

	void TcpMultiplexer::AcceptLoop() {
		while (m_running) {
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

			auto optEntry = m_connections.GetConnection(*optKey);
			if (!optEntry.has_value()) {
				session->Terminate();
				continue;
			}

			auto established = session->Establish(*optKey, *optEntry, [this](auto s) {
				if (auto tcpSession = std::dynamic_pointer_cast<TcpSession>(s)) {
					m_monitor.ReportFlowClosed(tcpSession->GetFlow());

					{
						std::lock_guard<std::mutex> lock(m_sessionsMtx);
						auto it = std::find(m_sessions.begin(), m_sessions.end(), tcpSession);
						if (it != m_sessions.end()) {
							m_sessions.erase(it);
						}
					}

					ThreeTuple key{
						.srcAddress = tcpSession->GetAddress(),
						.srcPort = tcpSession->GetPort(),
						.protocol = IPPROTO_TCP
					};

					auto optKey = m_connections.FindKey(key);
					if (optKey.has_value()) {
						m_connections.RemoveConnection(*optKey);
					}
				}
			});

			if (!established) {
				continue;
			}

			{
				std::lock_guard<std::mutex> lock(m_sessionsMtx);
				m_sessions.push_back(session);
			}
		}
	}

	std::optional<std::vector<FlowContract>> TcpMultiplexer::GetActiveFlows() {
		std::lock_guard<std::mutex> lock(m_sessionsMtx);
		if (m_sessions.empty()) {
			return std::nullopt;
		}

		std::vector<FlowContract> snapshot;
		snapshot.reserve(m_sessions.size());

		for (const auto& session : m_sessions) {
			if (session) {
				snapshot.push_back(session->GetFlow());
			}
		}

		return snapshot;
	}

	void TcpMultiplexer::TerminateFlow(const std::string& id) {
		std::shared_ptr<TcpSession> target;
		{
			std::lock_guard<std::mutex> lock(m_sessionsMtx);
			for (const auto& session : m_sessions) {
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