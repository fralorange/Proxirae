#include "features/persistence/domains/DomainTable.h"

namespace Proxirae {

	DomainTable::DomainTable(std::size_t maxCapacity, std::chrono::seconds defaultTtl)
		: m_maxCapacity(maxCapacity), m_defaultTtl(defaultTtl) {
	}

	void DomainTable::Add(const IpAddress& ip, std::string_view domain, std::uint32_t ttlSeconds)
	{
		std::unique_lock lock(m_mutex);

		auto ttl = (ttlSeconds > 0) ? std::chrono::seconds(ttlSeconds) : m_defaultTtl;
		auto expiresAt = std::chrono::steady_clock::now() + ttl;

		if (m_domains.size() >= m_maxCapacity && !m_domains.contains(ip)) {
			EvictExpiredOrOldestUnsafe();
		}

		m_domains.insert_or_assign(ip, DomainEntry{ std::string(domain), expiresAt });
	}

	void DomainTable::Remove(const IpAddress& ip)
	{
		std::unique_lock lock(m_mutex);
		m_domains.erase(ip);
	}

	std::optional<std::string> DomainTable::Get(const IpAddress& ip) const
	{
		std::shared_lock lock(m_mutex);

		auto it = m_domains.find(ip);
		if (it == m_domains.end()) {
			return std::nullopt;
		}

		if (std::chrono::steady_clock::now() > it->second.expiresAt) {
			return std::nullopt;
		}

		return it->second.domain;
	}

	void DomainTable::EvictExpiredOrOldestUnsafe()
	{
		auto now = std::chrono::steady_clock::now();

		for (auto it = m_domains.begin(); it != m_domains.end(); ++it) {
			if (now > it->second.expiresAt) {
				m_domains.erase(it);
				return;
			}
		}

		if (!m_domains.empty()) {
			m_domains.erase(m_domains.begin());
		}
	}
}