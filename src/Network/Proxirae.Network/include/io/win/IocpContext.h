#pragma once

#include <WinSock2.h>

#include "IoOperation.h"
#include "io/IoCallback.h"

namespace Proxirae {
	struct IocpContext {
		WSAOVERLAPPED overlapped;
		WSABUF wsaBuf;
		IoOperation operation;
		IoCallback callback;

		IocpContext(IoOperation operation, IoCallback callback);
	};
}