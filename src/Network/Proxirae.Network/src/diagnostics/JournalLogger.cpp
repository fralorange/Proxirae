#include "diagnostics/JournalLogger.h"
#include "contracts/log/LogContract.h"

namespace Proxirae {
	JournalLogger::JournalLogger(IpcMessenger& messenger, Store<Preferences>& store)
		: m_messenger(messenger), m_store(store) {}

	void JournalLogger::Log(LogLevel level, std::string_view message)
	{
		auto prefs = m_store.Get();

		LogLevel threshold = StringToLogLevel(prefs->logLevel);
		if (level < threshold) {
			return;
		}

		auto now = std::chrono::system_clock::now();

		std::lock_guard<std::mutex> lock(m_mutex);

		auto log = LogContract{
			.level = static_cast<LogLevelContract>(level),
			.message = message,
			.timestamp = now
		};

		m_messenger.Send(PipeMessageType::Evt_SendLog, log);
	}

	LogLevel JournalLogger::StringToLogLevel(std::string_view str)
	{
		static const std::unordered_map<std::string_view, LogLevel> map = {
			{"Debug", Proxirae::LogLevel::Debug},
			{"Info", Proxirae::LogLevel::Info},
			{"Warning", Proxirae::LogLevel::Warning},
			{"Error", Proxirae::LogLevel::Error},
			{"Critical", Proxirae::LogLevel::Critical}
		};

		auto it = map.find(str);
		if (it != map.end()) {
			return it->second;
		}

		return LogLevel::Info;
	}
}