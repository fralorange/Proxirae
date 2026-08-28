#pragma once

#include <vector>

#include "monitoring/IFlowMonitor.h"
#include "monitoring/IRoutingMonitor.h"
#include "contracts/flow/FlowContract.h"
#include "contracts/route/RouteContract.h"
#include "communication/IpcMessenger.h"

namespace Proxirae {
	class Monitor : public IFlowMonitor, public IRoutingMonitor {
	public:	
		Monitor(IpcMessenger& messenger);

		void ReportFlowSnapshot(std::vector<FlowContract> flows) override;
		void ReportFlowClosed(FlowContract flow) override;

		void ReportRouteEvent(RouteContract route) override;

	private:
		IpcMessenger& m_messenger;
	};
}