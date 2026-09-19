#include <cstring>

#include "asyncio/async/win/IocpContext.h"

namespace Proxirae {
	IocpContext::IocpContext(IoOperation operation)
		: operation(operation)
	{
		std::memset(&overlapped, 0, sizeof(OVERLAPPED));
	}
}