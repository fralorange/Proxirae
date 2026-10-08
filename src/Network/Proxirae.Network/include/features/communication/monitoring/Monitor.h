#pragma once

#include <vector>

#include "features/communication/monitoring/flow/IFlowMonitor.h"
#include "features/communication/monitoring/route/IRoutingMonitor.h"
#include "core/contracts/flow/FlowContract.h"
#include "core/contracts/route/RouteContract.h"
#include "features/communication/channels/messengers/IpcMessenger.h"

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