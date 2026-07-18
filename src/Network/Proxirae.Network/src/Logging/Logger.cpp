#pragma once

#include "Logging/ILogger.h"

namespace Proxirae::Network::Logging {
	void ILogger::LogDebug(std::string_view message)
	{
		Log(LogLevel::Debug, message);
	}

	void ILogger::LogInfo(std::string_view message)
	{
		Log(LogLevel::Info, message);
	}

	void ILogger::LogWarning(std::string_view message)
	{
		Log(LogLevel::Warning, message);
	}

	void ILogger::LogError(std::string_view message)
	{
		Log(LogLevel::Error, message);
	}

	void ILogger::LogCritical(std::string_view message)
	{
		Log(LogLevel::Critical, message);
	}
}