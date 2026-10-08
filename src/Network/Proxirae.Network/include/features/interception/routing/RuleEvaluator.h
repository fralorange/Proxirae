#pragma once

#include "core/contracts/rule/RuleContract.h"
#include "features/interception/diversion/IPacketContext.h"
#include "platform/processes/resolver/ProcessInfo.h"

#include "features/interception/routing/ProcessCriteria.h"
#include "features/interception/routing/PortCriteria.h"
#include "features/interception/routing/HostCriteria.h"

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