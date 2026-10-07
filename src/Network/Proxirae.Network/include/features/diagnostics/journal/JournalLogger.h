#pragma once

#include <mutex>
#include <string_view>

#include "features/diagnostics/ILogger.h"
#include "features/communication/channels/messengers/IpcMessenger.h"
#include "features/persistence/Store.h"
#include "features/persistence/preferences/Preferences.h"

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