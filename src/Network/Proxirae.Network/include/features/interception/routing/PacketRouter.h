#pragma once

#include <optional>

#include "features/interception/diversion/IPacketContext.h"
#include "core/contracts/rule/RuleActionContract.h"
#include "platform/processes/resolver/IProcessResolver.h"
#include "features/interception/routing/RuleEvaluator.h"
#include "features/persistence/connections/ConnectionTable.h"
#include "features/persistence/Store.h"
#include "features/persistence/configuration/Configuration.h"
#include "features/communication/monitoring/route/IRoutingMonitor.h"

namespace Proxirae {
	class PacketRouter {
	public:
		PacketRouter(IProcessResolver& resolver, RuleEvaluator& evaluator, Store<Configuration>& config, ConnectionTable& connections, IRoutingMonitor& monitor);

		RuleActionContract Route(IPacketContext& ctx);

	private:
		IProcessResolver& m_resolver;
		
		RuleEvaluator& m_evaluator;

		Store<Configuration>& m_config;

		ConnectionTable& m_connections;

		IRoutingMonitor& m_monitor;

		std::optional<RuleActionContract> TryGetExistingRoute(IPacketContext& ctx);
	};
}