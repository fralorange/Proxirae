#pragma once

#include "processes/CachedProcessResolver.h"
#include "processes/IProcessGuard.h"

namespace Proxirae {
	class WinProcessResolver : public CachedProcessResolver, public IProcessGuard {
	public:
		void AcquireProcess(std::uint32_t pid) override;
		void ReleaseProcess(std::uint32_t pid) override;

	protected:
		std::optional<ProcessInfo> ResolveProcess(std::uint32_t pid) override;

	private:
		std::unordered_map<uint32_t, uint32_t> m_refCount;
	};
}