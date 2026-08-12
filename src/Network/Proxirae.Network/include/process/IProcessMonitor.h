#pragma once

#include <cstdint>

namespace Proxirae {
	class IProcessMonitor {
	public:
		virtual ~IProcessMonitor() = default;

		virtual void AcquireProcess(std::uint32_t pid) = 0;
		virtual void ReleaseProcess(std::uint32_t pid) = 0;
	};
}