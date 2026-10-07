#pragma once

#include "platform/processes/manager/IProcessManager.h"

namespace Proxirae {
	class WinProcessManager : public IProcessManager {
		bool KillProcess(std::uint32_t pid) override;
	};
}