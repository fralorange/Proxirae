#pragma once

#include <optional>

#include "IPacketContext.h"
#include "contracts/rule/RuleActionContract.h"
#include "process/IProcessResolver.h"
#include "packet/RuleEvaluator.h"
#include "persistence/ConnectionTable.h"
#include "persistence/ConfigurationStore.h"

namespace Proxirae {
	class PacketRouter {
	public:
		PacketRouter(IProcessResolver& resolver, RuleEvaluator& evaluator, ConfigurationStore& config, ConnectionTable& connections);

		RuleActionContract Route(IPacketContext& ctx);

	private:
		IProcessResolver& m_resolver;
		
		RuleEvaluator& m_evaluator;

		ConfigurationStore& m_config;

		ConnectionTable& m_connections;

		std::optional<RuleActionContract> TryGetExistingRoute(IPacketContext& ctx);
	};
}