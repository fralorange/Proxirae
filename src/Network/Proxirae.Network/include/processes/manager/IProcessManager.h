#pragma once

#include <cstdint>

namespace Proxirae {
	class IProcessManager {
	public:
		virtual ~IProcessManager() = default;

		virtual bool KillProcess(std::uint32_t pid) = 0;
	};
}