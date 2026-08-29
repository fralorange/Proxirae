#pragma once

#include <mutex>
#include <string_view>

#include "diagnostics/ILogger.h"
#include "communication/IpcMessenger.h"
#include "persistence/Store.h"
#include "persistence/Preferences.h"

namespace Proxirae {
	class JournalLogger : public ILogger {
	public:
		JournalLogger(IpcMessenger& messenger, Store<Preferences>& store);

		void Log(LogLevel level, std::string_view message) override;

	private:
		IpcMessenger& m_messenger;
		Store<Preferences>& m_store;

		std::mutex m_mutex;

	private:
		LogLevel StringToLogLevel(std::string_view str);
	};
}