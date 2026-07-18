#pragma once

#include <mutex>

#include "Logging/ILogger.h"

namespace Proxirae::Network::Logging {
	class ConsoleLogger : public ILogger {
	public:
		void Log(LogLevel level, std::string_view message) override;

	private:
		std::mutex m_mutex;
	};
}