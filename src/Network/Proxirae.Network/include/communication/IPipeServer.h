#pragma once
#include <span>

#include "asyncio/IoCallback.h"

namespace Proxirae {
    class IPipeServer {
    public:
		virtual ~IPipeServer() = default;

		virtual bool Accept() = 0;

		virtual void AsyncRead(std::span<std::byte> buffer, IoCallback callback) = 0;
		virtual void AsyncWrite(std::span<const std::byte> buffer, IoCallback callback) = 0;
    };
}