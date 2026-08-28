#include "processes/CachedProcessResolver.h"

namespace Proxirae {
	std::optional<ProcessInfo> CachedProcessResolver::Resolve(std::uint32_t pid)
	{
		auto it = m_cache.find(pid);

		if (it != m_cache.end()) {
			return it->second;
		}

		auto info = ResolveProcess(pid);

		if (info.has_value()) {
			m_cache.emplace(pid, info.value());
		}

		return info;
	}

	void CachedProcessResolver::InvalidateCache(std::uint32_t pid)
	{
		m_cache.erase(pid);
	}
}