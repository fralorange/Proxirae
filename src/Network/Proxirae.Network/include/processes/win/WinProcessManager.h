#pragma once

#include "processes/IProcessManager.h"

namespace Proxirae {
	class WinProcessManager : public IProcessManager {
		bool KillProcess(std::uint32_t pid) override;
	};
}