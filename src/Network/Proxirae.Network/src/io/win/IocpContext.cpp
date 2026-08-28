#include "io/win/IocpContext.h"

namespace Proxirae {
	IocpContext::IocpContext(IoOperation operation, IoCallback callback)
		: operation(operation), callback(callback)
	{
		std::memset(&overlapped, 0, sizeof(OVERLAPPED));
	}
}