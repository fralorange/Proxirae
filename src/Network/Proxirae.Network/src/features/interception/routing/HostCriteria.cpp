#include <ranges>
#include <algorithm>
#include <charconv>
#include <cstring>

#include "features/interception/routing/HostCriteria.h"
#include "platform/environment/inet.h"
#include "utils/StringUtils.h"

namespace Proxirae {

	bool HostCriteria::IsMatch(const std::string& ruleHosts, const IpAddress& destinationIp) const
	{
		if (ruleHosts.empty()) {
			return true;
		}

		auto splitView = ruleHosts | std::views::split(';');

		for (const auto& part : splitView) {
			std::string_view token = StringUtils::Trim(std::string_view(part.begin(), part.end()));
			if (token.empty()) continue;

			auto slashPos = token.find('/');
			if (slashPos != std::string_view::npos) {
				auto ipStr = StringUtils::Trim(token.substr(0, slashPos));
				auto prefixStr = StringUtils::Trim(token.substr(slashPos + 1));

				auto netIp = ParseIp(ipStr);
				int prefixLen = 0;
				auto [ptr, ec] = std::from_chars(prefixStr.data(), prefixStr.data() + prefixStr.size(), prefixLen);

				if (netIp && ec == std::errc()) {
					if (MatchCidr(destinationIp, *netIp, prefixLen)) {
						return true;
					}
				}
				continue;
			}

			auto dashPos = token.find('-');
			if (dashPos != std::string_view::npos) {
				auto startStr = StringUtils::Trim(token.substr(0, dashPos));
				auto endStr = StringUtils::Trim(token.substr(dashPos + 1));

				auto startIp = ParseIp(startStr);
				auto endIp = ParseIp(endStr);

				if (startIp && endIp && MatchRange(destinationIp, *startIp, *endIp)) {
					return true;
				}
				continue;
			}

			if (token.find('*') != std::string_view::npos) {
				std::string startIpStr(token);
				std::string endIpStr(token);

				bool isIPv6 = token.find(':') != std::string_view::npos;
				std::string_view maxBlock = isIPv6 ? "ffff" : "255";

				std::ranges::replace(startIpStr, '*', '0');

				size_t pos = endIpStr.find('*');
				while (pos != std::string::npos) {
					endIpStr.replace(pos, 1, maxBlock);
					pos = endIpStr.find('*', pos + maxBlock.length());
				}

				auto startIp = ParseIp(startIpStr);
				auto endIp = ParseIp(endIpStr);

				if (startIp && endIp && MatchRange(destinationIp, *startIp, *endIp)) {
					return true;
				}
				continue;
			}

			auto ip = ParseIp(token);
			if (ip.has_value()) {
				if (destinationIp.isIPv6 == ip->isIPv6 && CompareIp(destinationIp, *ip) == 0) {
					return true;
				}
				continue;
			}
		}

		return false;
	}

	std::optional<IpAddress> HostCriteria::ParseIp(std::string_view ipStr) const
	{
		if (ipStr.empty() || ipStr.size() >= 64) {
			return std::nullopt;
		}

		char buf[64] = { 0 };
		std::memcpy(buf, ipStr.data(), ipStr.size());

		IpAddress addr{};

		if (inet_pton(AF_INET, buf, addr.data.data()) == 1) {
			addr.isIPv6 = false;
			return addr;
		}

		if (inet_pton(AF_INET6, buf, addr.data.data()) == 1) {
			addr.isIPv6 = true;
			return addr;
		}

		return std::nullopt;
	}

	int HostCriteria::CompareIp(const IpAddress& a, const IpAddress& b) const
	{
		size_t len = a.isIPv6 ? 16 : 4;
		const auto* ptrA = reinterpret_cast<const std::uint8_t*>(a.data.data());
		const auto* ptrB = reinterpret_cast<const std::uint8_t*>(b.data.data());
		return std::memcmp(ptrA, ptrB, len);
	}

	bool HostCriteria::MatchCidr(const IpAddress& dest, const IpAddress& net, int prefixLen) const
	{
		if (dest.isIPv6 != net.isIPv6) {
			return false;
		}

		int totalBits = dest.isIPv6 ? 128 : 32;
		if (prefixLen < 0 || prefixLen > totalBits) {
			return false;
		}

		const auto* ptrDest = reinterpret_cast<const std::uint8_t*>(dest.data.data());
		const auto* ptrNet = reinterpret_cast<const std::uint8_t*>(net.data.data());

		int fullBytes = prefixLen / 8;
		int remainingBits = prefixLen % 8;

		if (fullBytes > 0 && std::memcmp(ptrDest, ptrNet, fullBytes) != 0) {
			return false;
		}

		if (remainingBits > 0) {
			std::uint8_t mask = static_cast<std::uint8_t>(0xFF << (8 - remainingBits));
			if ((ptrDest[fullBytes] & mask) != (ptrNet[fullBytes] & mask)) {
				return false;
			}
		}

		return true;
	}

	bool HostCriteria::MatchRange(const IpAddress& dest, const IpAddress& start, const IpAddress& end) const
	{
		if (dest.isIPv6 != start.isIPv6 || dest.isIPv6 != end.isIPv6) {
			return false;
		}

		return CompareIp(dest, start) >= 0 && CompareIp(dest, end) <= 0;
	}
}