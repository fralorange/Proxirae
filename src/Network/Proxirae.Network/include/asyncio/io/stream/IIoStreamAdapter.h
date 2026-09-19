#pragma once

#include <span>

#include "environment/handle_types.h"
#include "asyncio/io/IoCallback.h"

namespace Proxirae {
	class IIoStreamAdapter {
	public:
		virtual ~IIoStreamAdapter() = default;

		virtual void AsyncRead(NativeHandle handle, std::span<std::byte> buffer, IoCallback callback) = 0;
		virtual void AsyncWrite(NativeHandle handle, std::span<const std::byte> buffer, IoCallback callback) = 0;
	};
}