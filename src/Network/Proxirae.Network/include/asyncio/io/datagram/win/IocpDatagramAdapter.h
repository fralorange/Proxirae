#pragma once

#include "asyncio/io/datagram/IIoDatagramAdapter.h"

namespace Proxirae {
	class IocpDatagramAdapter : public IIoDatagramAdapter {
	public:
		void AsyncRecvFrom(NativeHandle handle, std::span<std::byte> buffer, IoDatagramCallback callback) override;
		void AsyncSendTo(NativeHandle handle, const sockaddr* destAddr, NativeSocketLen destLen, std::span<const std::byte> buffer, IoDatagramCallback callback) override;
	};
}