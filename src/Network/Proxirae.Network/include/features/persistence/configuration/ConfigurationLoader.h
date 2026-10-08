#pragma once
#include <filesystem>

#include "features/persistence/Store.h"
#include "Configuration.h"
#include "features/diagnostics/ILogger.h"
#include "platform/protection/IProtector.h"
#include "LoadTarget.h"

namespace Proxirae {
	class ConfigurationLoader {
	public:
		ConfigurationLoader(Store<Configuration>& store, std::filesystem::path configDir, ILogger& logger, IProtector& protector);

		void Load(LoadTarget target = LoadTarget::Proxies | LoadTarget::Rules);
	private:
		Store<Configuration>& m_store;
		std::filesystem::path m_configDir;
		ILogger& m_logger;
		IProtector& m_protector;

		template<typename T>
		std::vector<T> ReadJson(const std::filesystem::path& path) const;
	};
}