#include <WinSock2.h>

#include "asyncio/async/win/IocpDriver.h"
#include "asyncio/async/win/IocpContext.h"

namespace Proxirae {
	IocpDriver::~IocpDriver()
	{
		Stop();
	}

	bool IocpDriver::Start(std::size_t threadCount)
	{
		if (m_running) {
			return true;
		}

		HANDLE handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
		if (!handle) {
			return false;
		}

		m_iocpHandle = static_cast<NativeHandle>(reinterpret_cast<std::uintptr_t>(handle));

		if (threadCount <= 0) {
			SYSTEM_INFO sysInfo;
			GetSystemInfo(&sysInfo);
			threadCount = sysInfo.dwNumberOfProcessors;
		}

		m_running = true;

		for (std::size_t i = 0; i < threadCount; ++i) {
			m_workers.emplace_back(&IocpDriver::Run, this);
		}

		return true;
	}

	void IocpDriver::Stop()
	{
		if (!m_running) {
			return;
		}

		m_running = false;
		auto handle = reinterpret_cast<HANDLE>(m_iocpHandle);

		for (std::size_t i = 0; i < m_workers.size(); ++i) {
			PostQueuedCompletionStatus(handle, 0, 0, nullptr);
		}

		for (auto& worker : m_workers) {
			if (worker.joinable()) {
				worker.join();
			}
		}
		m_workers.clear();

		if (handle) {
			CloseHandle(handle);
			m_iocpHandle = 0;
		}
	}

	bool IocpDriver::Attach(NativeHandle handle)
	{
		auto iocpHandle = reinterpret_cast<HANDLE>(m_iocpHandle);
		auto targetHandle = reinterpret_cast<HANDLE>(handle);

		HANDLE hResult = CreateIoCompletionPort(targetHandle, iocpHandle, 0, 0);
		return hResult != nullptr;
	}

	void IocpDriver::Run() {
		auto handle = reinterpret_cast<HANDLE>(m_iocpHandle);

		DWORD bytesTransferred = 0;
		ULONG_PTR completionKey = 0;
		LPOVERLAPPED pOverlapped = nullptr;

		while (m_running) {
			BOOL ok = GetQueuedCompletionStatus(handle, &bytesTransferred, &completionKey, &pOverlapped, INFINITE);

			if (!pOverlapped) {
				if (!m_running) break;
				continue;
			}

			auto* ctx = CONTAINING_RECORD(pOverlapped, IocpContext, overlapped);

			IoResult result{};
			if (ok) {
				result.success = true;
				result.bytesTransferred = bytesTransferred;
				result.errorCode = 0;
			}
			else {
				result.success = false;
				result.bytesTransferred = 0;
				result.errorCode = static_cast<int>(GetLastError());
			}

			ctx->Complete(result);

			delete ctx;
		}
	}
}