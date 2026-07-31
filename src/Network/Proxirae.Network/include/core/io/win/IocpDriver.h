#pragma once

#include <WinSock2.h>
#include <thread>

#include "core/io/IIoDriver.h"

namespace Proxirae {
	class IocpDriver : public IIoDriver {
	public:
		~IocpDriver() override;

		bool Start(std::size_t threadCount) override;
		void Stop() override;

		bool Attach(NativeHandle handle) override;

		void AsyncRead(NativeHandle handle, std::span<char> buffer, IoCallback callback) override;
		void AsyncWrite(NativeHandle handle, std::span<const char> buffer, IoCallback callback) override;

	private:
		void Run();

		HANDLE m_iocpHandle{ nullptr };
		std::vector<std::thread> m_workers;
		std::atomic_bool m_running{ false };
	};
}