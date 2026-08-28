#pragma once

#include <span>

#include "IoCallback.h"

namespace Proxirae {
	using NativeHandle = std::uintptr_t;

	class IIoDriver {
	public:
		virtual ~IIoDriver() = default;

		virtual bool Start(std::size_t threadCount = 0) = 0;
		virtual void Stop() = 0;

		virtual bool Attach(NativeHandle handle) = 0;

		virtual void AsyncRead(NativeHandle handle, std::span<std::byte> buffer, IoCallback callback) = 0;
		virtual void AsyncWrite(NativeHandle handle, std::span<const std::byte> buffer, IoCallback callback) = 0;
	};
}