#include <iostream>

#include "Logging/ConsoleLogger.h"

#define CYAN    "\033[36m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define RED     "\033[31m"
#define BOLDRED     "\033[1m\033[31m"
#define RESET   "\033[0m"

namespace Proxirae::Network::Logging {
	void ConsoleLogger::Log(LogLevel level, std::string_view message)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		switch (level) {
			case LogLevel::Debug:
				std::clog << CYAN << "[DEBUG] " << RESET << message << std::endl;
				break;
			case LogLevel::Info:
				std::clog << GREEN << "[INFO] " << RESET << message << std::endl;
				break;
			case LogLevel::Warning:
				std::cerr << YELLOW << "[WARNING] " << RESET << message << std::endl;
				break;
			case LogLevel::Error:
				std::cerr << RED << "[ERROR] " << RESET << message << std::endl;
				break;
			case LogLevel::Critical:
				std::cerr << BOLDRED << "[CRITICAL] " << RESET << message << std::endl;
				break;
		}
	}
}