#pragma once

#include <WinSock2.h>
#include <vector>

#include "asyncio/async/win/IocpContext.h"
#include "asyncio/io/datagram/IoDatagramCallback.h"

namespace Proxirae {
	struct IocpDatagramContext : public IocpContext {
		IoDatagramCallback callback;
		sockaddr_storage remoteAddr{};
		int remoteAddrLen{ sizeof(sockaddr_storage) };
		WSABUF wsaBuf{};
		DWORD flags{ 0 };
		std::vector<std::byte> buffer;

		IocpDatagramContext(IoOperation operation, IoDatagramCallback callback);

		void Complete(const IoResult& baseRes) override;
	};
}