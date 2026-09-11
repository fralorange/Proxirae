#pragma once

#include <thread>

#include "transport/IMultiplexer.h"
#include "UdpBinder.h"
#include "persistence/connections/ConnectionTable.h"
#include "monitoring/flow/IFlowMonitor.h"
#include "proxification/IProxyFactory.h"
#include "diagnostics/ILogger.h"
#include "asyncio/io/datagram/IIoDatagramAdapter.h"
#include "UdpSession.h"

namespace Proxirae {
	class UdpMultiplexer : public IMultiplexer {
	public:
		UdpMultiplexer(UdpBinder& binder, IIoDatagramAdapter& adapter, ConnectionTable& connections, IFlowMonitor& monitor, IProxyFactory& factory, ILogger& logger);
		~UdpMultiplexer();

		bool Start() override;
		bool Stop() override;

		std::optional<std::vector<FlowContract>> GetActiveFlows() override;
		void TerminateFlow(const std::string& id) override;

	private:
		class UdpProcessor;
		std::unique_ptr<UdpProcessor> m_processor;
	};
}