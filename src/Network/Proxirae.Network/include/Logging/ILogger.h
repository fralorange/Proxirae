#pragma once

#include <string_view>

#include "LogLevel.h"

namespace Proxirae::Network::Logging {
	class ILogger {
	public:
		virtual ~ILogger() = default;

		virtual void Log(LogLevel level, std::string_view message) = 0;
		void LogDebug(std::string_view message);
		void LogInfo(std::string_view message);
		void LogWarning(std::string_view message);
		void LogError(std::string_view message);
		void LogCritical(std::string_view message);
	};
}