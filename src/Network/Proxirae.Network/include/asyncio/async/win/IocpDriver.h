#pragma once

#include <thread>
#include <vector>
#include <atomic>

#include "asyncio/async/IAsyncDriver.h"
#include "features/diagnostics/ILogger.h"

namespace Proxirae {
	class IocpDriver : public IAsyncDriver {
	public:
		explicit IocpDriver(ILogger& logger);
		~IocpDriver() override;

		bool Start(std::size_t threadCount) override;
		void Stop() override;

		bool Attach(NativeHandle handle) override;

	private:
		void Run();

		ILogger& m_logger;

		NativeHandle m_iocpHandle{ 0 };
		std::vector<std::thread> m_workers;
		std::atomic_bool m_running{ false };
	};
}