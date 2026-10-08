#pragma once

#include <vector>
#include <functional>
#include <stop_token>
#include <string>

#include "features/transport/IMultiplexer.h"
#include "features/diagnostics/ILogger.h"
#include "features/communication/monitoring/flow/IFlowMonitor.h"

namespace Proxirae {
	class Daemon {
	public:
		using ReadyCallback = std::function<void(bool)>;

		Daemon(IMultiplexer& multiplexer, ILogger& logger, IFlowMonitor& monitor, std::stop_token token);
		~Daemon();

		void Run(ReadyCallback onReady);

		std::optional<std::vector<FlowContract>> GetActiveFlows();
		void TerminateFlow(std::string id);
	private:
		IMultiplexer& m_multiplexer;
		ILogger& m_logger;
		IFlowMonitor& m_monitor;

		std::stop_token m_token;
	};
}