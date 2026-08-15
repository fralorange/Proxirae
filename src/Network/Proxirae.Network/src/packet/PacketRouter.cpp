#include "packet/PacketRouter.h"

namespace Proxirae {
	PacketRouter::PacketRouter(IProcessResolver& resolver, RuleEvaluator& evaluator, ConfigurationStore& config, ConnectionTable& connections)
		: m_resolver(resolver), m_evaluator(evaluator), m_config(config), m_connections(connections) { }

	RuleActionContract PacketRouter::Route(IPacketContext& ctx)
	{
		auto existingActionOpt = TryGetExistingRoute(ctx);

		if (existingActionOpt.has_value()) {
			return existingActionOpt.value();
		}
		
		RuleActionContract defaultAction{ RuleAction::Direct, "" };

		auto pidOpt = ctx.GetProcessId();

		if (!pidOpt.has_value()) {
			return defaultAction;
		}

		auto pid = pidOpt.value();

		auto infoOpt = m_resolver.Resolve(pid);

		if (!infoOpt.has_value()) {
			return defaultAction;
		}

		auto& info = infoOpt.value();

		auto configuration = m_config.Get();

		for (const auto& rule : configuration->rules) {
			if (!rule.isEnabled) {
				continue;
			}

			if (m_evaluator.IsMatch(rule, ctx, info)) {
				return rule.action;
			}
		}

		return defaultAction;
	}

	std::optional<RuleActionContract> PacketRouter::TryGetExistingRoute(IPacketContext& ctx)
	{
		ThreeTuple outKey{
			.srcAddress = ctx.GetSourceAddress(),
			.srcPort = ctx.GetSourcePort(),
			.protocol = ctx.GetProtocol()
		};

		auto optOutKey = m_connections.FindKey(outKey);

		if (optOutKey.has_value()) {
			auto outIt = m_connections.GetConnection(*optOutKey);
			if (outIt.has_value()) {
				return RuleActionContract{ RuleAction::Proxy, outIt->get().proxyId };
			}
		}

		ThreeTuple inKey{
			.srcAddress = ctx.GetDestinationAddress(),
			.srcPort = ctx.GetDestinationPort(),
			.protocol = ctx.GetProtocol()
		};

		auto optInKey = m_connections.FindKey(inKey);

		if (optInKey.has_value()) {
			auto inIt = m_connections.GetConnection(*optInKey);
			if (inIt.has_value()) {
				return RuleActionContract{ RuleAction::Proxy, inIt->get().proxyId };
			}
		}

		return std::nullopt;
	}
}