#pragma once

#include "asyncio/io/stream/IIoStreamAdapter.h"

namespace Proxirae {
	class IocpStreamAdapter : public IIoStreamAdapter {
	public:
		void AsyncRead(NativeHandle handle, std::span<std::byte> buffer, IoCallback callback) override;
		void AsyncWrite(NativeHandle handle, std::span<const std::byte> buffer, IoCallback callback) override;
	};
}