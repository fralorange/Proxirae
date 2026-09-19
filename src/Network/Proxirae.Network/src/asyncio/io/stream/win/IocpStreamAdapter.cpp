#include "WinSock2.h"

#include "asyncio/io/stream/win/IocpStreamAdapter.h"
#include "asyncio/io/stream/win/IocpStreamContext.h"

namespace Proxirae {
	void IocpStreamAdapter::AsyncRead(NativeHandle handle, std::span<std::byte> buffer, IoCallback callback)
	{
		auto* ctx = new IocpStreamContext{ IoOperation::Read, std::move(callback) };

		BOOL result = ReadFile(
			reinterpret_cast<HANDLE>(handle),
			buffer.data(),
			static_cast<DWORD>(buffer.size()),
			nullptr,
			&ctx->overlapped
		);

		if (!result) {
			DWORD err = GetLastError();

			if (err != ERROR_IO_PENDING) {
				ctx->Complete(IoResult{ false, 0, static_cast<int>(err) });
				delete ctx;
			}
		}
	}

	void IocpStreamAdapter::AsyncWrite(NativeHandle handle, std::span<const std::byte> buffer, IoCallback callback)
	{
		auto* ctx = new IocpStreamContext(IoOperation::Write, std::move(callback));
		ctx->buffer.assign(buffer.begin(), buffer.end());

		BOOL result = WriteFile(
			reinterpret_cast<HANDLE>(handle),
			ctx->buffer.data(),
			static_cast<DWORD>(ctx->buffer.size()),
			nullptr,
			&ctx->overlapped
		);

		if (!result) {
			DWORD err = GetLastError();

			if (err != ERROR_IO_PENDING) {
				ctx->Complete(IoResult{ false, 0, static_cast<int>(err) });
				delete ctx;
			}
		}
	}
}