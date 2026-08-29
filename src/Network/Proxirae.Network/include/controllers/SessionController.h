#pragma once

#include <string>
#include <cstdint>

#include "runtime/Daemon.h"
#include "processes/IProcessManager.h"

namespace Proxirae {
	class SessionController {
	public:
		SessionController(Daemon& daemon, IProcessManager& manager);

		void DropSession(std::string id);
		void KillSessionProcess(std::uint32_t processId);

	private:
		Daemon& m_daemon;
		IProcessManager& m_manager;
	};
}