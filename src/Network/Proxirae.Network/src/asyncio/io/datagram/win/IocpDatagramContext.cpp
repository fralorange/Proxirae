#include "asyncio/io/datagram/win/IocpDatagramContext.h"

namespace Proxirae {
	IocpDatagramContext::IocpDatagramContext(IoOperation operation, IoDatagramCallback callback)
		: IocpContext(operation), callback(std::move(callback)) { }

	void IocpDatagramContext::Complete(const IoResult& baseRes)
	{
		if (!callback) return;

		IoDatagramResult result{};
		result.success = baseRes.success;
		result.bytesTransferred = baseRes.bytesTransferred;
		result.errorCode = baseRes.errorCode;
		result.remoteAddr = remoteAddr;
		result.remoteAddrLen = remoteAddrLen;

		callback(result);
	}
}