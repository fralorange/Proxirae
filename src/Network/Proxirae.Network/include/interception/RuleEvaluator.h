#pragma once

#include "contracts/rule/RuleContract.h"
#include "interception/IPacketContext.h"
#include "processes/ProcessInfo.h"

#include "interception/ProcessCriteria.h"
#include "interception/PortCriteria.h"
#include "interception/HostCriteria.h"

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