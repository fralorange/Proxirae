#include "asyncio/io/datagram/win/IocpDatagramAdapter.h"
#include "asyncio/io/datagram/win/IocpDatagramContext.h"

namespace Proxirae {
	void IocpDatagramAdapter::AsyncRecvFrom(NativeHandle handle, std::span<std::byte> buffer, IoDatagramCallback callback)
	{
		auto* ctx = new IocpDatagramContext(IoOperation::Read, std::move(callback));
		ctx->wsaBuf.buf = reinterpret_cast<CHAR*>(buffer.data());
		ctx->wsaBuf.len = static_cast<ULONG>(buffer.size());

		ctx->remoteAddrLen = sizeof(ctx->remoteAddr);

		int result = WSARecvFrom(
			static_cast<SOCKET>(handle),
			&ctx->wsaBuf,
			1,
			nullptr,
			&ctx->flags,
			reinterpret_cast<sockaddr*>(&ctx->remoteAddr),
			&ctx->remoteAddrLen,
			&ctx->overlapped,
			nullptr
		);

		if (result == SOCKET_ERROR) {
			int err = WSAGetLastError();

			if (err != WSA_IO_PENDING) {
				ctx->Complete(IoResult{ false, 0, err });
				delete ctx;
			}
		}
	}

	void IocpDatagramAdapter::AsyncSendTo(NativeHandle handle, const sockaddr* destAddr, NativeSocketLen destLen, std::span<const std::byte> buffer, IoDatagramCallback callback)
	{
		auto* ctx = new IocpDatagramContext(IoOperation::Write, std::move(callback));
		ctx->buffer.assign(buffer.begin(), buffer.end());
		ctx->wsaBuf.buf = reinterpret_cast<CHAR*>(ctx->buffer.data());
		ctx->wsaBuf.len = static_cast<ULONG>(ctx->buffer.size());

		int result = WSASendTo(
			static_cast<SOCKET>(handle),
			&ctx->wsaBuf,
			1,
			nullptr,
			0,
			destAddr, 
			destLen, 
			&ctx->overlapped,
			nullptr
		);

		if (result == SOCKET_ERROR) {
			int err = WSAGetLastError();

			if (err != WSA_IO_PENDING) {
				ctx->Complete(IoResult{ false, 0, err });
				delete ctx;
			}
		}
	}
}