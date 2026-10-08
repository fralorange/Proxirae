#pragma once

#include <mutex>

#include "features/diagnostics/ILogger.h"

namespace Proxirae {
	class ConsoleLogger : public ILogger {
	public:
		void Log(LogLevel level, std::string_view message) override;

	private:
		std::mutex m_mutex;
	};
}