#include <string>
#include <format>
#include <WS2tcpip.h>
#include <thread>

#include "core/Daemon.h"
#include "registry/ConnectionKey.h"
#include "core/IProxy.h"
#include "core/Socks5Proxy.h"

constexpr uint16_t DAEMON_PORT = 33999;

namespace Proxirae {
	Daemon::Daemon(TcpListener& listener, ConnectionTable& connections, ILogger& logger)
		: m_listener(listener), m_connections(connections), m_logger(logger) {}

	Daemon::~Daemon() {
		Stop();
	}

	void Daemon::Start() {
		
		m_listener.Listen(DAEMON_PORT);
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
				session->Stop();
				continue;
			}

			AddClient(session);

			std::thread([session, entry = it.value(), this]() {
				session->Handle(entry);

				this->RemoveClient(session);
			}).detach();
		}
	}

	void Daemon::Stop() {
		m_running = false;

		std::lock_guard<std::mutex> lock(m_sessions_mtx);

		for (auto& session : m_sessions) {
			if (session) {
				session->Stop();
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