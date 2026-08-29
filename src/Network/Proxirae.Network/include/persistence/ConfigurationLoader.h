#pragma once
#include <filesystem>

#include "Store.h"
#include "Configuration.h"
#include "diagnostics/ILogger.h"
#include "LoadTarget.h"

namespace Proxirae {
	class ConfigurationLoader {
	public:
		ConfigurationLoader(Store<Configuration>& store, std::filesystem::path configDir, ILogger& logger);

		void Load(LoadTarget target = LoadTarget::Proxies | LoadTarget::Rules);
	private:
		Store<Configuration>& m_store;
		std::filesystem::path m_configDir;
		ILogger& m_logger;

		template<typename T>
		std::vector<T> ReadJson(const std::filesystem::path& path) const;
	};
}