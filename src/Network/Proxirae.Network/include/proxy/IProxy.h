#pragma once

#include <string_view>
#include <span>
#include <functional>

#include "io/IoResult.h"

namespace Proxirae {
	class IProxy {
	public:
		virtual ~IProxy() = default;

		virtual bool Connect(std::string_view targetAddress, std::uint16_t port) = 0;
		virtual void Disconnect() = 0;

		virtual void Send(std::span<const std::byte> buffer, std::function<void(const IoResult&)> callback) = 0;
		virtual void Recv(std::span<std::byte> buffer, std::function<void(const IoResult&)> callback) = 0;
	};
}