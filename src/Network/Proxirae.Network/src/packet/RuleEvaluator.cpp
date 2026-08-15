#include "packet/RuleEvaluator.h"
#include "platform/inet.h"

namespace Proxirae {
    bool RuleEvaluator::MatchProtocol(RuleProtocol ruleProto, IPacketContext& ctx) const {
        if (ruleProto == RuleProtocol::None) return false;

        using T = std::underlying_type_t<RuleProtocol>;
        if (ctx.IsTcp() && (static_cast<T>(ruleProto) & static_cast<T>(RuleProtocol::TCP))) return true;
        if (ctx.IsUdp() && (static_cast<T>(ruleProto) & static_cast<T>(RuleProtocol::UDP))) return true;

        return false;
    }

    bool RuleEvaluator::IsMatch(const RuleContract& rule, IPacketContext& ctx, const ProcessInfo& processInfo) const {

        if (!MatchProtocol(rule.protocol, ctx)) {
            return false;
        }

        if (!m_portCriteria.IsMatch(rule.ports, ntohs(ctx.GetDestinationPort()))) {
            return false;
        }

        if (!m_processCriteria.IsMatch(rule.processes, processInfo)) {
            return false;
        }

        if (!m_hostCriteria.IsMatch(rule.hosts, ntohl(ctx.GetDestinationAddress()))) {
            return false;
        }

        return true;
    }
}