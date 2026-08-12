#pragma once

#include <vector>
#include <mutex>

#include "core/transport/TcpSession.h"
#include "core/transport/TcpListener.h"
#include "core/registry/ConnectionRegistry.h"
#include "diagnostics/ILogger.h"

namespace Proxirae {
	class Daemon {
	public:
		Daemon(TcpListener& listener, ConnectionRegistry& connections, ILogger& logger);
		~Daemon();

		void Start(std::uint16_t port, std::function<void(bool)> onReady);
		void Stop();

	private:
		TcpListener m_listener;
		std::vector<std::shared_ptr<TcpSession>> m_sessions;
		std::mutex m_sessions_mtx;

		ConnectionRegistry& m_connections;

		ILogger& m_logger;

		std::atomic_bool m_running{ false };

		void AddClient(std::shared_ptr<TcpSession> session);
		void RemoveClient(std::shared_ptr<TcpSession> session);
	};
}