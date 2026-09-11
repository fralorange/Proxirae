#pragma once

#include <cstdint>

namespace Proxirae {
	class IProcessGuard {
	public:
		virtual ~IProcessGuard() = default;

		virtual void AcquireProcess(std::uint32_t pid) = 0;
		virtual void ReleaseProcess(std::uint32_t pid) = 0;
	};
}