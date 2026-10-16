#pragma once

#include <Windows.h>
#include <exception>

#include "asyncio/io/IoOperation.h"
#include "asyncio/io/IoResult.h"

namespace Proxirae {
	struct IocpContext {
		OVERLAPPED overlapped;
		IoOperation operation;
		
		explicit IocpContext(IoOperation operation);
		virtual ~IocpContext() = default;

		[[nodiscard]]
		std::exception_ptr Complete(const IoResult& result) noexcept;

	protected:
		virtual void OnComplete(const IoResult& result) = 0;
	};
}