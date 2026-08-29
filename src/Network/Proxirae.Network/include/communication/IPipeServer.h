#pragma once
#include <span>
#include <stop_token>

#include "asyncio/IoCallback.h"

namespace Proxirae {
    class IPipeServer {
    public:
		virtual ~IPipeServer() = default;

		virtual bool Accept(std::stop_token token) = 0;

		virtual void AsyncRead(std::span<std::byte> buffer, IoCallback callback) = 0;
		virtual void AsyncWrite(std::span<const std::byte> buffer, IoCallback callback) = 0;
    };
}