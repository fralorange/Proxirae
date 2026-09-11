#pragma once

#include <thread>
#include <vector>
#include <atomic>

#include "asyncio/async/IAsyncDriver.h"

namespace Proxirae {
	class IocpDriver : public IAsyncDriver {
	public:
		~IocpDriver() override;

		bool Start(std::size_t threadCount) override;
		void Stop() override;

		bool Attach(NativeHandle handle) override;

	private:
		void Run();

		NativeHandle m_iocpHandle{ 0 };
		std::vector<std::thread> m_workers;
		std::atomic_bool m_running{ false };
	};
}