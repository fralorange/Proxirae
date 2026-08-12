#include <Windows.h>
#include <filesystem>

#include "process/win/WinProcessResolver.h"

namespace Proxirae {
	void WinProcessResolver::AcquireProcess(std::uint32_t pid)
	{
		++m_refCount[pid];
	}

	void WinProcessResolver::ReleaseProcess(std::uint32_t pid)
	{
		auto it = m_refCount.find(pid);

		if (it == m_refCount.end()) {
			return;
		}

		if (--it->second == 0) {
			m_refCount.erase(it);
			InvalidateCache(pid);
		}
	}

	std::optional<ProcessInfo> WinProcessResolver::ResolveProcess(std::uint32_t pid)
	{
		HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, static_cast<DWORD>(pid));

		if (process == nullptr) {
			return std::nullopt;
		}

		wchar_t buffer[MAX_PATH];
		DWORD size = MAX_PATH;

		const BOOL success = QueryFullProcessImageNameW(process, 0, buffer, &size);

		CloseHandle(process);

		if (!success) {
			return std::nullopt;
		}

		std::filesystem::path path(buffer);

		ProcessInfo info{
			.name = path.filename().string(),
			.path = path.string()
		};

		return info;
	}
}