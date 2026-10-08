#pragma once

#include <span>
#include <string_view>
#include <cstdint>
#include <functional>
#include <string>

#include "asyncio/io/IoCallback.h"

namespace Proxirae {
	class IDatagramProxy {
	public:
		virtual ~IDatagramProxy() = default;

		virtual bool Connect() = 0;
		virtual void Disconnect() = 0;

		virtual void Send(
			std::span<const std::byte> buffer,
			std::string_view targetAddress,
			std::uint16_t targetPort,
			IoCallback callback
		) = 0;
		virtual void Recv(
			std::span<std::byte> buffer,
			std::function<void(const IoResult& result, std::string sourceAddress, std::uint16_t sourcePort)> callback
		) = 0;
	};
}