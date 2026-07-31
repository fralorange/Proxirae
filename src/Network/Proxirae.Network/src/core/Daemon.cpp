#include <string>
#include <format>

#include "core/Daemon.h"
#include "platform/inet.h"
#include "core/registry/ConnectionKey.h"

namespace Proxirae {
	Daemon::Daemon(TcpListener& listener, ConnectionTable& connections, ILogger& logger)
		: m_listener(listener), m_connections(connections), m_logger(logger) {
	}

	Daemon::~Daemon() {
		Stop();
	}

	void Daemon::Start(std::uint16_t port, std::function<void(bool)> onReady) {
		bool success = m_listener.Listen(port);

		if (!success) {
			m_logger.LogError("Failed to start Daemon. Err=Listener closed.");
			
			return;
		}

		onReady(success);

		m_running = true;

		while (m_running) {
			auto session = m_listener.Accept();

			if (!session) {
				continue;
			}

			ConnectionKey key{
				.srcAddress = session->GetAddress(),
				.srcPort = session->GetPort(),
				.protocol = IPPROTO_TCP
			};

			auto it = m_connections.GetConnection(key);

			if (!it.has_value()) {
				session->Terminate();
				continue;
			}

			AddClient(session);

			session->Handle(it.value(), [this](auto s) {
				RemoveClient(s);
			});
		}
	}

	void Daemon::Stop() {
		m_running = false;

		std::lock_guard<std::mutex> lock(m_sessions_mtx);

		for (auto& session : m_sessions) {
			if (session) {
				session->Terminate();
			}
		}

		m_sessions.clear();
	}

	void Daemon::AddClient(std::shared_ptr<TcpSession> session)
	{
		std::lock_guard<std::mutex> lock(m_sessions_mtx);
		m_sessions.push_back(session);
	}

	void Daemon::RemoveClient(std::shared_ptr<TcpSession> session)
	{
		std::lock_guard<std::mutex> lock(m_sessions_mtx);
		auto it = std::find(m_sessions.begin(), m_sessions.end(), session);
		if (it != m_sessions.end()) {
			m_sessions.erase(it);
		}

		ConnectionKey key{
			.srcAddress = session->GetAddress(),
			.srcPort = session->GetPort(),
			.protocol = IPPROTO_TCP
		};

		m_connections.RemoveConnection(key);
	}
}