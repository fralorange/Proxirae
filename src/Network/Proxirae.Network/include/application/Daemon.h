#pragma once

#include <vector>
#include <mutex>
#include <stop_token>
#include <string>

#include "transport/TcpSession.h"
#include "transport/TcpListener.h"
#include "persistence/ConnectionTable.h"
#include "diagnostics/ILogger.h"
#include "monitoring/IFlowMonitor.h"

namespace Proxirae {
	class Daemon {
	public:
		Daemon(TcpListener& listener, ConnectionTable& connections, ILogger& logger, IFlowMonitor& monitor, std::stop_token token);
		~Daemon();

		void Run(std::uint16_t port, std::function<void(bool)> onReady);

		std::optional<std::vector<FlowContract>> GetActiveFlows();
		void TerminateFlow(std::string id);
	private:
		TcpListener m_listener;
		std::vector<std::shared_ptr<TcpSession>> m_sessions;
		std::mutex m_sessions_mtx;

		ConnectionTable& m_connections;

		ILogger& m_logger;

		IFlowMonitor& m_monitor;

		std::stop_token m_token;
	};
}