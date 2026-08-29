#pragma once

#include <Windows.h>

#include "IoOperation.h"
#include "asyncio/IoCallback.h"

namespace Proxirae {
	struct IocpContext {
		OVERLAPPED overlapped;
		IoOperation operation;
		IoCallback callback;
		std::vector<std::byte> buffer;

		IocpContext(IoOperation operation, IoCallback callback);
	};
}