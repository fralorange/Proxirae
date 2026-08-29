#pragma once

#include <filesystem>

#include "Store.h"
#include "Preferences.h"
#include "diagnostics/ILogger.h"

namespace Proxirae {
	class PreferencesLoader {
	public:
		PreferencesLoader(Store<Preferences>& store, std::filesystem::path prefsDir, ILogger& logger);

		void Load();

	private:
		Store<Preferences>& m_store;
		std::filesystem::path m_prefsDir;
		ILogger& m_logger;
	};
}