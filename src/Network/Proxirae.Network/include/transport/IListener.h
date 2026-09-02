#pragma once

#include <cstdint>
#include <memory>

#include "ISession.h"

namespace Proxirae {
	template<typename T>
	requires std::derived_from<T, ISession>
	class IListener {
	public:
		virtual ~IListener() = default;

		virtual std::uint16_t Bind() = 0;
		virtual bool Listen(std::uint16_t port) = 0;

		virtual std::shared_ptr<T> Accept() = 0;

		virtual void Close() = 0;
	};
}