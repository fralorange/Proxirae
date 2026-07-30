#pragma once

#include <vector>
#include <mutex>
#include <WinSock2.h>

#include "core/TcpSession.h"
#include "core/TcpListener.h"
#include "registry/ConnectionTable.h"
#include "diagnostics/ILogger.h"

namespace Proxirae {
	class Daemon {
	public:
		Daemon(TcpListener& listener, ConnectionTable& connections, ILogger& logger);
		~Daemon();

		void Start();
		void Stop();

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