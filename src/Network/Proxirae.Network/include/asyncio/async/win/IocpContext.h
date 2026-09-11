#pragma once

#include <Windows.h>

#include "asyncio/io/IoOperation.h"
#include "asyncio/io/IoResult.h"

namespace Proxirae {
	struct IocpContext {
		OVERLAPPED overlapped;
		IoOperation operation;
		
		explicit IocpContext(IoOperation operation);
		virtual ~IocpContext() = default;

		virtual void Complete(const IoResult& result) = 0;
	};
}