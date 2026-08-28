#include "controllers/SessionController.h"

namespace Proxirae {
	SessionController::SessionController(Daemon& daemon, IProcessManager& manager)
	: m_daemon(daemon), m_manager(manager) { }

	void SessionController::DropSession(std::string id)
	{
		m_daemon.TerminateFlow(id);
	}

	void SessionController::KillSessionProcess(std::uint32_t processId)
	{
		m_manager.KillProcess(processId);
	}
}