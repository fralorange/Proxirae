#include "utils/PathUtils.h"

namespace Proxirae::PathUtils {
	std::filesystem::path GetAppConfigDirectory()
	{
		std::filesystem::path appDataPath;

#ifdef _WIN32

		wchar_t* appData = nullptr;
		size_t len = 0;

		if (_wdupenv_s(&appData, &len, L"APPDATA") == 0 && appData != nullptr) {
			appDataPath = appData;
			free(appData);
		}
		else {
			throw std::runtime_error("APPDATA_ENV_NULL");
		}

#else

		const char* home = std::getenv("HOME");
		if (home == nullptr) {
			throw std::runtime_error("HOME_ENV_NULL");
		}

		appDataPath = std::filesystem::path(home) / ".config";
#endif 

		std::filesystem::path configDir = appDataPath / "Proxirae";

		if (!std::filesystem::exists(configDir)) {
			std::filesystem::create_directories(configDir);
		}

		return configDir;
	}
}


