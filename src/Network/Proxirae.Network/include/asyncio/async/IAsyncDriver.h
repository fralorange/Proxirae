#pragma once

#include <cstdint>
#include <cstddef>

#include "environment/handle_types.h"

namespace Proxirae {
	class IAsyncDriver {
	public:
		virtual ~IAsyncDriver() = default;

		virtual bool Start(std::size_t threadCount = 0) = 0;
		virtual void Stop() = 0;

		virtual bool Attach(NativeHandle handle) = 0;
	};
}