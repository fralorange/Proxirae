#pragma once

#include <optional>

#include "IPacketContext.h"
#include "contracts/rule/RuleActionContract.h"
#include "process/IProcessResolver.h"
#include "packet/RuleEvaluator.h"
#include "persistence/ConnectionTable.h"
#include "persistence/Store.h"
#include "persistence/Configuration.h"
#include "monitoring/IRoutingMonitor.h"

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