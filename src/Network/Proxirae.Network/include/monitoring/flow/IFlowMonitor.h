#include <vector>

#include "contracts/flow/FlowContract.h"

#pragma once

namespace Proxirae {
	class IFlowMonitor {
	public:
		virtual ~IFlowMonitor() = default;

		virtual void ReportFlowSnapshot(std::vector<FlowContract> flows) = 0;
		virtual void ReportFlowClosed(FlowContract flow) = 0;
	};
}