#include "packet/PacketRouter.h"

namespace Proxirae {
	PacketRouter::PacketRouter(IProcessResolver& resolver, RuleEvaluator& evaluator, Store<Configuration>& config, ConnectionTable& connections, IRoutingMonitor& monitor)
		: m_resolver(resolver), m_evaluator(evaluator), m_config(config), m_connections(connections), m_monitor(monitor) { }

	RuleActionContract PacketRouter::Route(IPacketContext& ctx)
	{
		auto existingActionOpt = TryGetExistingRoute(ctx);

		if (existingActionOpt.has_value()) {
			return existingActionOpt.value();
		}
		
		RuleActionContract returnAction{ RuleAction::Direct, "" };

		RouteContract route{
			.timestamp = std::chrono::system_clock::now(),
			.address = ctx.GetDestinationAddress(),
			.port = ctx.GetDestinationPort(),
		};

		auto pidOpt = ctx.GetProcessId();

		if (pidOpt.has_value()) {
			auto infoOpt = m_resolver.Resolve(*pidOpt);

			route.processId = *pidOpt;
			
			if (infoOpt.has_value()) {
				auto& info = *infoOpt;
				auto config = m_config.Get();

				for (const auto& rule : config->rules) {
					if (rule.isEnabled && m_evaluator.IsMatch(rule, ctx, info)) {
						returnAction = rule.action;
						route.ruleId = rule.id;

						m_monitor.ReportRouteEvent(route);

						break;
					}
				}
			}
		}

		return returnAction;
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