#pragma once

#include <span>

#include "platform/environment/handle_types.h"
#include "IoDatagramCallback.h"
#include "platform/environment/sock_types.h"

namespace Proxirae {
	class IIoDatagramAdapter {
	public:
		virtual ~IIoDatagramAdapter() = default;

		virtual void AsyncRecvFrom(NativeHandle handle, std::span<std::byte> buffer, IoDatagramCallback callback) = 0;
		virtual void AsyncSendTo(NativeHandle handle, const sockaddr* destAddr, NativeSocketLen destLen, std::span<const std::byte> buffer, IoDatagramCallback callback) = 0;
	};
}