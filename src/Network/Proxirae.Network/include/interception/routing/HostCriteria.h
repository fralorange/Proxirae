#pragma once
#include <string>
#include <optional>

namespace Proxirae {
    class HostCriteria {
    public:
        bool IsMatch(const std::string& ruleHosts, std::uint32_t destinationIp) const;
    private:
        std::optional<std::uint32_t> ParseIp(std::string_view ipStr) const;
    };
}