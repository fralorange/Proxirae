#pragma once

#include <vector>
#include <mutex>

#include "transport/TcpSession.h"
#include "transport/TcpListener.h"
#include "persistence/ConnectionTable.h"
#include "diagnostics/ILogger.h"

namespace Proxirae {
	class Daemon {
	public:
		Daemon(TcpListener& listener, ConnectionTable& connections, ILogger& logger);
		~Daemon();

		void Run(std::uint16_t port, std::function<void(bool)> onReady);

	private:
		TcpListener m_listener;
		std::vector<std::shared_ptr<TcpSession>> m_sessions;
		std::mutex m_sessions_mtx;

		ConnectionTable& m_connections;

		ILogger& m_logger;

		std::atomic_bool m_running{ false };

		void AddClient(std::shared_ptr<TcpSession> session);
		void RemoveClient(std::shared_ptr<TcpSession> session);
	};
}