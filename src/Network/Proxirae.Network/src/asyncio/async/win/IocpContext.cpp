#include <cstring>

#include "asyncio/async/win/IocpContext.h"

namespace Proxirae {
	IocpContext::IocpContext(IoOperation operation)
		: operation(operation)
	{
		std::memset(&overlapped, 0, sizeof(OVERLAPPED));
	}

    std::exception_ptr IocpContext::Complete(const IoResult& result) noexcept
    {
        try {
            OnComplete(result);
            return {};
        }
        catch (...) {
            return std::current_exception();
        }
    }
}