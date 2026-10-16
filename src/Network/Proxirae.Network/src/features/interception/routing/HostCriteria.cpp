#include <ranges>
#include <algorithm>
#include <charconv>
#include <cstring>

#include "features/interception/routing/HostCriteria.h"
#include "platform/environment/inet.h"
#include "utils/StringUtils.h"

namespace Proxirae {

	bool HostCriteria::IsMatch(const std::string& ruleHosts, const IpAddress& destinationIp, std::optional<std::string_view> domain) const
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

			if (domain.has_value() && MatchDomain(token, *domain)) {
				return true;
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
			addr.data[0] = ntohl(addr.data[0]);
			return addr;
		}

		if (inet_pton(AF_INET6, buf, addr.data.data()) == 1) {
			addr.isIPv6 = true;

			for (auto& word : addr.data) {
				word = ntohl(word);
			}

			return addr;
		}

		return std::nullopt;
	}

	int HostCriteria::CompareIp(const IpAddress& a, const IpAddress& b) const
	{
		if (a.isIPv6 != b.isIPv6) {
			return a.isIPv6 ? 1 : -1;
		}

		const std::size_t wordCount = a.isIPv6 ? 4 : 1;

		for (std::size_t i = 0; i < wordCount; ++i) {
			if (a.data[i] < b.data[i]) {
				return -1;
			}

			if (a.data[i] > b.data[i]) {
				return 1;
			}
		}

		return 0;
	}

	bool HostCriteria::MatchDomain(std::string_view pattern, std::string_view domain) const
	{
		if (pattern == "*") {
			return true;
		}

		if (pattern.starts_with("*.")) {
			std::string_view suffix = pattern.substr(1);
			return domain == suffix.substr(1) || domain.ends_with(suffix);
		}

		auto starPos = pattern.find('*');
		if (starPos != std::string::npos) {
			std::string_view prefix = pattern.substr(0, starPos);
			std::string_view suffix = pattern.substr(starPos + 1);
			return domain.starts_with(prefix) && domain.ends_with(suffix) &&
				domain.size() >= prefix.size() + suffix.size();
		}

		return domain == pattern || domain.ends_with(std::string(1, '.') + std::string(pattern));
	}

	bool HostCriteria::MatchCidr(const IpAddress& dest, const IpAddress& net, int prefixLen) const
	{
		if (dest.isIPv6 != net.isIPv6) {
			return false;
		}

		const int totalBits = dest.isIPv6 ? 128 : 32;

		if (prefixLen < 0 || prefixLen > totalBits) {
			return false;
		}

		const std::size_t wordCount = dest.isIPv6 ? 4 : 1;
		int remainingBits = prefixLen;

		for (std::size_t i = 0; i < wordCount; ++i) {
			if (remainingBits == 0) {
				return true;
			}

			const int bits = (std::min)(remainingBits, 32);

			const std::uint32_t mask =
				bits == 32
				? 0xFFFFFFFFu
				: (0xFFFFFFFFu << (32 - bits));

			if ((dest.data[i] & mask) != (net.data[i] & mask)) {
				return false;
			}

			remainingBits -= bits;
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