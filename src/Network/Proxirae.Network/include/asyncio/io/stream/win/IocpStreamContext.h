#pragma once

#include <vector>

#include "asyncio/async/win/IocpContext.h"
#include "asyncio/io/IoCallback.h"

namespace Proxirae {
	struct IocpStreamContext : public IocpContext {
		IoCallback callback;
		std::vector<std::byte> buffer;

		IocpStreamContext(IoOperation operation, IoCallback callback);

		void Complete(const IoResult& result) override;
	};
}