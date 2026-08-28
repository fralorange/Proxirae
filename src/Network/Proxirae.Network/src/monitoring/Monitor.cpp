#include "monitoring/Monitor.h"

namespace Proxirae {
	Monitor::Monitor(IpcMessenger& messenger)
		: m_messenger(messenger) { }

	void Monitor::ReportFlowSnapshot(std::vector<FlowContract> flows)
	{
		if (!flows.empty()) {
			m_messenger.Send(PipeMessageType::Evt_SendFlow, std::move(flows));
		}
	}

	void Monitor::ReportFlowClosed(FlowContract flow)
	{
		flow.status = FlowStatus::Closed;
		m_messenger.Send(PipeMessageType::Evt_SendFlow, std::vector<FlowContract>{ std::move(flow) });
	}

	void Monitor::ReportRouteEvent(RouteContract route)
	{
		m_messenger.Send(PipeMessageType::Evt_SendRoute, std::move(route));
	}
}