#pragma once

#include <cstdint>
#include <mutex>
#include <thread>

#include "transport/IMultiplexer.h"
#include "TcpListener.h"
#include "persistence/connections/ConnectionTable.h"
#include "monitoring/flow/IFlowMonitor.h"
#include "diagnostics/ILogger.h"

namespace Proxirae {
	class TcpMultiplexer : public IMultiplexer {
	public:
		TcpMultiplexer(TcpListener& listener, ConnectionTable& connections, IFlowMonitor& monitor, ILogger& logger, std::uint16_t port);
		~TcpMultiplexer() override;

		bool Start() override;
		bool Stop() override;

		std::optional<std::vector<FlowContract>> GetActiveFlows() override;
		void TerminateFlow(const std::string& id) override;

	private:
		void AcceptLoop();

		TcpListener& m_listener;
		ConnectionTable& m_connections;
		IFlowMonitor& m_monitor;
		ILogger& m_logger;
		std::uint16_t m_port;

		std::vector<std::shared_ptr<TcpSession>> m_sessions;
		std::mutex m_sessionsMtx;

		std::atomic_bool m_running{ false };
		std::thread m_acceptThread;
	};
}