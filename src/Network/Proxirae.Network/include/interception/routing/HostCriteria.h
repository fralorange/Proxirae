#pragma once

#include <string>
#include <string_view>
#include <optional>

#include "primitives/ip/IpAddress.h"

namespace Proxirae {
	class HostCriteria {
	public:
		bool IsMatch(const std::string& ruleHosts, const IpAddress& destinationIp) const;

	private:
		std::optional<IpAddress> ParseIp(std::string_view ipStr) const;
		int CompareIp(const IpAddress& a, const IpAddress& b) const;

	private:
		bool MatchCidr(const IpAddress& dest, const IpAddress& net, int prefixLen) const;
		bool MatchRange(const IpAddress& dest, const IpAddress& start, const IpAddress& end) const;
	};
}