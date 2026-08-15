#pragma once

#include "contracts/rule/RuleContract.h"
#include "packet/IPacketContext.h"
#include "process/ProcessInfo.h"

#include "packet/ProcessCriteria.h"
#include "packet/PortCriteria.h"
#include "packet/HostCriteria.h"

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