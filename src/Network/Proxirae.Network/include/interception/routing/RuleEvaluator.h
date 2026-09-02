#pragma once

#include "contracts/rule/RuleContract.h"
#include "interception/diversion/IPacketContext.h"
#include "processes/ProcessInfo.h"

#include "interception/routing/ProcessCriteria.h"
#include "interception/routing/PortCriteria.h"
#include "interception/routing/HostCriteria.h"

namespace Proxirae {
	class RuleEvaluator {
    public:
        bool IsMatch(const RuleContract& rule, IPacketContext& ctx, const ProcessInfo& processInfo) const;

    private:
        bool MatchProtocol(RuleProtocol ruleProto, IPacketContext& ctx) const;

        ProcessCriteria m_processCriteria{};
        PortCriteria m_portCriteria{};
        HostCriteria m_hostCriteria{};
	};
}