#pragma once
#include <string>

#include "platform/processes/resolver/ProcessInfo.h"

namespace Proxirae {
    class ProcessCriteria {
    public:
        bool IsMatch(const std::string& ruleProcesses, const ProcessInfo& processInfo) const;
    };
}