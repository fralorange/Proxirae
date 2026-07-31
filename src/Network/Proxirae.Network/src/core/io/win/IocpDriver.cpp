#include "core/io/win/IocpDriver.h"
#include "core/io/win/IocpContext.h"

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

		m_iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
		if (!m_iocpHandle) {
			return false;
		}

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

		for (std::size_t i = 0; i < m_workers.size(); ++i) {
			PostQueuedCompletionStatus(m_iocpHandle, 0, 0, nullptr);
		}

		for (auto& worker : m_workers) {
			if (worker.joinable()) {
				worker.join();
			}
		}
		m_workers.clear();

		if (m_iocpHandle) {
			CloseHandle(m_iocpHandle);
			m_iocpHandle = nullptr;
		}
	}

	bool IocpDriver::Attach(NativeHandle handle)
	{
		HANDLE hResult = CreateIoCompletionPort(reinterpret_cast<HANDLE>(handle), m_iocpHandle, 0, 0);

		return hResult != nullptr;
	}

	void IocpDriver::AsyncRead(NativeHandle handle, std::span<char> buffer, IoCallback callback)
	{
		auto* ctx = new IocpContext(IoOperation::Read, std::move(callback));
		ctx->wsaBuf.buf = buffer.data();
		ctx->wsaBuf.len = static_cast<ULONG>(buffer.size());

		SOCKET sock = static_cast<SOCKET>(handle);
		DWORD flags = 0;
		DWORD bytesRecv = 0;

		int result = WSARecv(sock, &ctx->wsaBuf, 1, &bytesRecv, &flags, &ctx->overlapped, NULL);

		if (result == SOCKET_ERROR) {
			int err = WSAGetLastError();

			if (err != WSA_IO_PENDING) {
				ctx->callback(IoResult{ false, 0, err });
				delete ctx;
			}
		}
	}

	void IocpDriver::AsyncWrite(NativeHandle handle, std::span<const char> buffer, IoCallback callback)
	{
		auto* ctx = new IocpContext(IoOperation::Write, std::move(callback));
		ctx->wsaBuf.buf = const_cast<char*>(buffer.data());
		ctx->wsaBuf.len = static_cast<ULONG>(buffer.size());

		SOCKET sock = static_cast<SOCKET>(handle);
		DWORD bytesSent = 0;

		int result = WSASend(sock, &ctx->wsaBuf, 1, &bytesSent, 0, &ctx->overlapped, NULL);

		if (result == SOCKET_ERROR) {
			int err = WSAGetLastError();

			if (err != WSA_IO_PENDING) {
				ctx->callback(IoResult{ false, 0, err });
				delete ctx;
			}
		}
	}

	void IocpDriver::Run() {
		DWORD bytesTransferred = 0;
		ULONG_PTR completionKey = 0;
		LPOVERLAPPED pOverlapped = nullptr;

		while (m_running) {
			BOOL ok = GetQueuedCompletionStatus(m_iocpHandle, &bytesTransferred, &completionKey, &pOverlapped, INFINITE);

			if (!pOverlapped) {
				if (!m_running) break;
				continue;
			}

			auto* ctx = reinterpret_cast<IocpContext*>(pOverlapped);

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

			ctx->callback(result);

			delete ctx;
		}
	}
}