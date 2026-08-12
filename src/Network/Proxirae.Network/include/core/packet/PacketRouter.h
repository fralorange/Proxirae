#pragma once

#include <set>
#include <optional>

#include "IPacketContext.h"
#include "contracts/rule/RuleActionContract.h"
#include "contracts/rule/RuleContract.h"
#include "process/IProcessResolver.h"
#include "utils/rule/RuleEvaluator.h"
#include "core/registry/ConnectionRegistry.h"

namespace Proxirae {
	class PacketRouter {
	public:
		PacketRouter(IProcessResolver& resolver, RuleEvaluator& evaluator, std::set<RuleContract>& rules, ConnectionRegistry& connections);

		RuleActionContract Route(IPacketContext& ctx);

	private:
		IProcessResolver& m_resolver;
		
		RuleEvaluator& m_evaluator;

		std::set<RuleContract>& m_rules;

		ConnectionRegistry& m_connections;

		std::optional<RuleActionContract> TryGetExistingRoute(IPacketContext& ctx);
	};
}