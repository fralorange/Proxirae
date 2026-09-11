#pragma once

#include <string_view>
#include <span>
#include <functional>

#include "asyncio/io/IoCallback.h"

namespace Proxirae {
	class IStreamProxy {
	public:
		virtual ~IStreamProxy() = default;

		virtual bool Connect(std::string_view targetAddress, std::uint16_t port) = 0;
		virtual void Disconnect() = 0;

		virtual void Send(std::span<const std::byte> buffer, IoCallback callback) = 0;
		virtual void Recv(std::span<std::byte> buffer, IoCallback callback) = 0;
	};
}