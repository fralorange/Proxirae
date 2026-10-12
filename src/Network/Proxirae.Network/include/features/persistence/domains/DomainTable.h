#pragma once

#include <unordered_map>
#include <string_view>
#include <optional>
#include <shared_mutex>	

#include "features/persistence/domains/DomainEntry.h"
#include "core/primitives/ip/IpAddress.h"
#include "core/primitives/ip/IpAddressHash.h"

namespace Proxirae {
	class DomainTable {
	public:
		explicit DomainTable(std::size_t capacity = 10000, std::chrono::seconds ttl = std::chrono::seconds(300));

		void Add(const IpAddress& ip, const std::string_view domain, std::uint32_t ttlSeconds);
		void Remove(const IpAddress& ip);

		std::optional<std::string> Get(const IpAddress& ip) const;

	private:
		void EvictExpiredOrOldestUnsafe();

	private:
		mutable std::shared_mutex m_mutex;
		std::unordered_map<IpAddress, DomainEntry, IpAddressHash> m_domains;

		std::size_t m_maxCapacity;
		std::chrono::seconds m_defaultTtl;
	};
}