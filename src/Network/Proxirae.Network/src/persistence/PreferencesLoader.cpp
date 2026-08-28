#include <nlohmann/json.hpp>
#include <fstream>

#include "persistence/PreferencesLoader.h"
#include "utils/JsonUtils.h"

namespace Proxirae {
	PreferencesLoader::PreferencesLoader(Store<Preferences>& store, std::filesystem::path prefsDir, ILogger& logger)
		: m_store(store), m_prefsDir(prefsDir), m_logger(logger) { }

	void PreferencesLoader::Load()
	{
        auto prefsPath = m_prefsDir / "preferences.json";

        try {
            if (!std::filesystem::exists(prefsPath) || std::filesystem::file_size(prefsPath) == 0) {
                return;
            }

            std::ifstream file(prefsPath);
            if (!file.is_open()) return;

            nlohmann::json j;
            file >> j;

            if (j.contains("Engine")) {
                auto enginePrefs = j.at("Engine").get<Preferences>();
                m_store.Update(std::make_shared<Preferences>(std::move(enginePrefs)));
            }
        }
        catch (const std::exception& e) {
            m_logger.LogError(std::format("[Preferences] Failed to load {}: {}", prefsPath.filename().string(), e.what()));
        }
	}
}