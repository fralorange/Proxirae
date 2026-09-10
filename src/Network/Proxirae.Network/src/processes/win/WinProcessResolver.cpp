#include <Windows.h>
#include <TlHelp32.h>

#include "processes/win/WinProcessResolver.h"
#include "utils/StringUtils.h"

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

		if (process != nullptr) {
			wchar_t buffer[MAX_PATH];
			DWORD size = MAX_PATH;

			const BOOL success = QueryFullProcessImageNameW(process, 0, buffer, &size);
			CloseHandle(process);

			if (success) {
				std::filesystem::path path(buffer);
				return ProcessInfo{
					.name = StringUtils::ToUTF8(path.filename()),
					.path = StringUtils::ToUTF8(path)
				};
			}
		}

		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (snapshot == INVALID_HANDLE_VALUE) {
			return std::nullopt;
		}

		PROCESSENTRY32W entry{};
		entry.dwSize = sizeof(entry);

		if (Process32FirstW(snapshot, &entry)) {
			do {
				if (entry.th32ProcessID == pid) {
					CloseHandle(snapshot);
					std::filesystem::path path(entry.szExeFile);

					return ProcessInfo{
						.name = StringUtils::ToUTF8(path.filename()),
						.path = StringUtils::ToUTF8(path)
					};
				}
			} while (Process32NextW(snapshot, &entry));
		}

		CloseHandle(snapshot);
		return std::nullopt;
	}
}