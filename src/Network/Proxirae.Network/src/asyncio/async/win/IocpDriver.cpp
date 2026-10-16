#include <WinSock2.h>
#include <memory>
#include <format>
#include <exception>
#include <new>
#include <stdexcept>
#include <system_error>

#include "asyncio/async/win/IocpDriver.h"
#include "asyncio/async/win/IocpContext.h"

namespace Proxirae {
	IocpDriver::IocpDriver(ILogger& logger)
		: m_logger(logger) { }

	IocpDriver::~IocpDriver()
	{
		Stop();
	}

	bool IocpDriver::Start(std::size_t threadCount)
	{
		if (m_running) {
			return true;
		}

		HANDLE handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);

		if (!handle) {
			const DWORD error = GetLastError();

			m_logger.LogCritical(std::format(
				"[IocpDriver] Failed to create completion port: error {}",
				error
			));

			return false;
		}

		if (threadCount == 0) {
			SYSTEM_INFO sysInfo{};
			GetSystemInfo(&sysInfo);
			threadCount = sysInfo.dwNumberOfProcessors;

			if (threadCount == 0) {
				threadCount = 1;
			}
		}

		m_iocpHandle = static_cast<NativeHandle>(reinterpret_cast<std::uintptr_t>(handle));

		const char* stage = "reserving worker storage";

		try {
			m_workers.reserve(threadCount);

			m_running.store(true);
			stage = "creating worker threads";

			for (std::size_t i = 0; i < threadCount; ++i) {
				m_workers.emplace_back(&IocpDriver::Run, this);
			}

			return true;
		}
		catch (const std::system_error& e) {
			const auto startedWorkers = m_workers.size();

			Stop();

			m_logger.LogCritical(std::format(
				"[IocpDriver] Failed while {}: started {}/{} worker threads; "
				"system error: {} (code={}, category={}, message={})",
				stage,
				startedWorkers,
				threadCount,
				e.what(),
				e.code().value(),
				e.code().category().name(),
				e.code().message()
			));

			return false;
		}
		catch (const std::bad_alloc& e) {
			const auto startedWorkers = m_workers.size();

			Stop();

			m_logger.LogCritical(std::format(
				"[IocpDriver] Out of memory while {}: started {}/{} worker threads; {}",
				stage,
				startedWorkers,
				threadCount,
				e.what()
			));

			return false;
		}
		catch (const std::length_error& e) {
			const auto startedWorkers = m_workers.size();

			Stop();

			m_logger.LogCritical(std::format(
				"[IocpDriver] Invalid worker storage size while {}: "
				"requested {} worker threads; {}",
				stage,
				threadCount,
				e.what()
			));

			return false;
		}
		catch (const std::exception& e) {
			const auto startedWorkers = m_workers.size();

			Stop();

			m_logger.LogCritical(std::format(
				"[IocpDriver] Standard exception while {}: started {}/{} worker threads; {}",
				stage,
				startedWorkers,
				threadCount,
				e.what()
			));

			return false;
		}
		catch (...) {
			const auto startedWorkers = m_workers.size();

			Stop();

			m_logger.LogCritical(std::format(
				"[IocpDriver] Non-standard exception while {}: "
				"started {}/{} worker threads",
				stage,
				startedWorkers,
				threadCount
			));

			return false;
		}
	}

	void IocpDriver::Stop()
	{
		if (!m_running.exchange(false)
			&& m_iocpHandle == 0
			&& m_workers.empty()) {
			return;
		}

		auto handle = reinterpret_cast<HANDLE>(m_iocpHandle);

		if (handle) {
			for (std::size_t i = 0; i < m_workers.size(); ++i) {
				PostQueuedCompletionStatus(
					handle,
					0,
					0,
					nullptr
				);
			}
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

		if (!hResult) {
			const DWORD error = GetLastError();

			m_logger.LogError(std::format(
				"[IocpDriver] Failed to attach handle: error {}",
				error
			));
		}

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

			auto ctx = std::unique_ptr<IocpContext>(
				CONTAINING_RECORD(
					pOverlapped,
					IocpContext,
					overlapped
				)
			);

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

			if (const auto callbackException = ctx->Complete(result)) {
				try {
					std::rethrow_exception(callbackException);
				}
				catch (const std::exception& e) {
					m_logger.LogError(std::format(
						"[IocpDriver] Exception in IO completion callback: {} "
						"(ioSuccess={}, bytesTransferred={}, ioError={})",
						e.what(),
						result.success,
						result.bytesTransferred,
						result.errorCode
					));
				}
				catch (...) {
					m_logger.LogError(std::format(
						"[IocpDriver] IO completion callback threw a non-standard exception "
						"(ioSuccess={}, bytesTransferred={}, ioError={})",
						result.success,
						result.bytesTransferred,
						result.errorCode
					));
				}
			}
		}
	}
}