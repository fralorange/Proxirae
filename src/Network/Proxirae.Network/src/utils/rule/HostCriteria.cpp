#include <ranges>
#include <algorithm>
#include <charconv>

#include "utils/rule/HostCriteria.h"
#include "utils/StringUtils.h"

namespace Proxirae {
    bool HostCriteria::IsMatch(const std::string& ruleHosts, std::uint32_t destinationIp) const {
        if (ruleHosts.empty()) return true;

        auto splitView = ruleHosts | std::views::split(';');

        for (const auto& part : splitView) {
            std::string_view token = StringUtils::Trim(std::string_view(part.begin(), part.end()));
            if (token.empty()) continue;

            if (std::ranges::any_of(token, [](char c) { return std::isalpha(c); })) {
                continue;
            }

            auto dashPos = token.find('-');
            if (dashPos != std::string_view::npos) {
                auto startStr = StringUtils::Trim(token.substr(0, dashPos));
                auto endStr = StringUtils::Trim(token.substr(dashPos + 1));

                auto startIp = ParseIp(startStr);
                auto endIp = ParseIp(endStr);

                if (startIp && endIp && destinationIp >= startIp.value() && destinationIp <= endIp.value()) {
                    return true;
                }
            }
            else if (token.find('*') != std::string_view::npos) {
                std::string startIpStr(token);
                std::string endIpStr(token);
                std::ranges::replace(startIpStr, '*', '0');

                size_t pos = endIpStr.find('*');
                while (pos != std::string::npos) {
                    endIpStr.replace(pos, 1, "255");
                    pos = endIpStr.find('*', pos + 3);
                }

                auto startIp = ParseIp(startIpStr);
                auto endIp = ParseIp(endIpStr);

                if (startIp && endIp && destinationIp >= startIp.value() && destinationIp <= endIp.value()) {
                    return true;
                }
            }
            else {
                auto ip = ParseIp(token);
                if (ip && destinationIp == ip.value()) {
                    return true;
                }
            }
        }
        return false;
    }

    std::optional<std::uint32_t> HostCriteria::ParseIp(std::string_view ipStr) const {
        // to host byte order
        std::uint32_t result = 0;
        int octets = 0;

        auto parts = ipStr | std::views::split('.');
        for (const auto& part : parts) {
            if (octets >= 4) return std::nullopt;

            std::uint8_t octet = 0;
            std::string_view octetStr(part.begin(), part.end());
            auto [ptr, ec] = std::from_chars(octetStr.data(), octetStr.data() + octetStr.size(), octet);

            if (ec != std::errc()) return std::nullopt;

            result = (result << 8) | octet;
            octets++;
        }

        return (octets == 4) ? std::make_optional(result) : std::nullopt;
    }
}