#pragma once
#include <string>

namespace Proxirae {
    class PortCriteria {
    public:
        bool IsMatch(const std::string& rulePorts, std::uint16_t destinationPort) const;
    };
}