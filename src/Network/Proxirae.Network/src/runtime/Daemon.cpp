#include <string>
#include <thread>

#include "runtime/Daemon.h"

namespace Proxirae {
	Daemon::Daemon(IMultiplexer& multiplexer, ILogger& logger, IFlowMonitor& monitor, std::stop_token token)
		: m_multiplexer(multiplexer),
		  m_logger(logger),
		  m_monitor(monitor),
		  m_token(token) { }

	Daemon::~Daemon() {
		m_multiplexer.Stop();
	}

	void Daemon::Run(ReadyCallback onReady) {
		bool success = m_multiplexer.Start();
		onReady(success);

		while (!m_token.stop_requested()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}

	std::optional<std::vector<FlowContract>> Daemon::GetActiveFlows()
	{
		return m_multiplexer.GetActiveFlows();
	}

	void Daemon::TerminateFlow(std::string id)
	{
		m_multiplexer.TerminateFlow(id);
	}
}