#include "asyncio/io/stream/win/IocpStreamContext.h"

namespace Proxirae {
	IocpStreamContext::IocpStreamContext(IoOperation operation, IoCallback callback)
		: IocpContext(operation), callback(std::move(callback)) { }

	void IocpStreamContext::Complete(const IoResult& result)
	{
		if (callback) {
			callback(result);
		}
	}
}