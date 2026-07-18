#pragma once

#include <vector>
#include <WinSock2.h>

#include "Persistence/ConnectionTable.h"
#include "Logging/ILogger.h"

namespace Proxirae::Network {
	class Daemon {
	public:
		Daemon(Persistence::ConnectionTable& connections, Logging::ILogger& logger);
		~Daemon();

		void Start();
		void Stop();

	private:
		SOCKET m_listenSocket;
		std::vector<SOCKET> m_clients;

		Persistence::ConnectionTable& m_connections;

		Logging::ILogger& m_logger;

		std::atomic_bool m_running;
	};
}