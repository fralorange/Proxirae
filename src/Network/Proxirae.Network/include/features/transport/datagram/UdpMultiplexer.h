#pragma once

#include <thread>

#include "features/transport/IMultiplexer.h"
#include "UdpBinder.h"
#include "features/persistence/connections/ConnectionTable.h"
#include "features/persistence/virtuals/VirtualTable.h"
#include "features/communication/monitoring/flow/IFlowMonitor.h"
#include "features/proxification/IProxyFactory.h"
#include "features/diagnostics/ILogger.h"
#include "asyncio/io/datagram/IIoDatagramAdapter.h"
#include "UdpSession.h"

namespace Proxirae {
	class UdpMultiplexer : public IMultiplexer {
	public:
		UdpMultiplexer(UdpBinder& binder, IIoDatagramAdapter& adapter, ConnectionTable& connections, VirtualTable& virtuals, IFlowMonitor& monitor, IProxyFactory& factory, ILogger& logger);
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