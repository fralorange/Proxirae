#pragma once

#include <unordered_map>

#include "processes/IProcessResolver.h"

namespace Proxirae {
	class CachedProcessResolver : public IProcessResolver {
	public:
		std::optional<ProcessInfo> Resolve(std::uint32_t pid) override;

	protected:
		virtual std::optional<ProcessInfo> ResolveProcess(std::uint32_t pid) = 0;
		void InvalidateCache(std::uint32_t pid);

	private:
		std::unordered_map<std::uint32_t, ProcessInfo> m_cache;
	};
}