#include <ranges>
#include <charconv>

#include "packet/PortCriteria.h"
#include "utils/StringUtils.h"

namespace Proxirae {
    bool PortCriteria::IsMatch(const std::string& rulePorts, std::uint16_t destinationPort) const {
        if (rulePorts.empty()) return true;

        auto splitView = rulePorts | std::views::split(';');

        for (const auto& part : splitView) {
            std::string_view token = StringUtils::Trim(std::string_view(part.begin(), part.end()));
            if (token.empty()) continue;

            auto dashPos = token.find('-');
            if (dashPos != std::string_view::npos) {
                std::uint16_t startPort = 0, endPort = 0;
                auto startStr = StringUtils::Trim(token.substr(0, dashPos));
                auto endStr = StringUtils::Trim(token.substr(dashPos + 1));

                std::from_chars(startStr.data(), startStr.data() + startStr.size(), startPort);
                std::from_chars(endStr.data(), endStr.data() + endStr.size(), endPort);

                if (destinationPort >= startPort && destinationPort <= endPort) return true;
            }
            else {
                std::uint16_t port = 0;
                std::from_chars(token.data(), token.data() + token.size(), port);

                if (destinationPort == port) return true;
            }
        }
        return false;
    }
}