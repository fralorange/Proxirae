#include <Windows.h>

#include "processes/win/WinProcessManager.h"

namespace Proxirae {
	bool WinProcessManager::KillProcess(std::uint32_t pid)
	{
		if (pid == 0) return false;

		HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);

		if (hProcess == NULL) {
			return GetLastError() == ERROR_INVALID_PARAMETER;
		}

		auto result = TerminateProcess(hProcess, 1);
		CloseHandle(hProcess);

		return result;
	}
}