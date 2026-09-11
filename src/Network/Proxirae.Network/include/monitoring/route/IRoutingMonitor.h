#pragma once

#include "contracts/route/RouteContract.h"

namespace Proxirae {
	class IRoutingMonitor {
	public:
		virtual ~IRoutingMonitor() = default;

		virtual void ReportRouteEvent(RouteContract route) = 0;
	};
}