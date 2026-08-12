#pragma once
#include <string>

#include "process/ProcessInfo.h"

namespace Proxirae {
    class ProcessCriteria {
    public:
        bool IsMatch(const std::string& ruleProcesses, const ProcessInfo& processInfo) const;
    };
}