#include <ranges>

#include "interception/ProcessCriteria.h"
#include "utils/StringUtils.h"

namespace Proxirae {
    bool ProcessCriteria::IsMatch(const std::string& ruleProcesses, const ProcessInfo& processInfo) const {
        if (ruleProcesses.empty()) return true;

        auto splitView = ruleProcesses | std::views::split(';');

        for (const auto& part : splitView) {
            std::string_view token = StringUtils::Trim(std::string_view(part.begin(), part.end()));
            if (token.empty()) continue;

            token = StringUtils::TrimQuotes(token);

            std::string_view targetToCompare = (token.find('\\') != std::string_view::npos || token.find('/') != std::string_view::npos)
                ? processInfo.path
                : processInfo.name;

            if (StringUtils::WildcardMatch(targetToCompare, token)) {
                return true;
            }
        }
        return false;
    }
}