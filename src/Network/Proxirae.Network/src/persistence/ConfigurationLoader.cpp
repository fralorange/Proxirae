#include <nlohmann/json.hpp>
#include <fstream>

#include "utils/JsonUtils.h"
#include "persistence/ConfigurationLoader.h"

namespace Proxirae {
	ConfigurationLoader::ConfigurationLoader(Store<Configuration>& store, std::filesystem::path configDir, ILogger& logger)
		: m_store(store), m_configDir(configDir), m_logger(logger) { }

	void ConfigurationLoader::Load(LoadTarget target)
	{
		if (target == LoadTarget::None) {
			return;
		}

		auto currentConfig = m_store.Get();
		auto newConfig = currentConfig 
			? std::make_shared<Configuration>(*currentConfig) 
			: std::make_shared<Configuration>();

		if ((target & LoadTarget::Proxies) != LoadTarget::None) {
			newConfig->proxies.clear();

			auto proxiesPath = m_configDir / "proxies.json";

			auto tempProxies = ReadJson<ProxyContract>(proxiesPath);
			for (auto& proxy : tempProxies) {
				newConfig->proxies.emplace(proxy.id, std::move(proxy));
			}
		}


		if ((target & LoadTarget::Rules) != LoadTarget::None) {
			newConfig->rules.clear();

			auto rulesPath = m_configDir / "rules.json";

			auto tempRules = ReadJson<RuleContract>(rulesPath);
			for (auto& rule : tempRules) {
				newConfig->rules.insert(std::move(rule));
			}
		}

		m_store.Update(newConfig);
	}

	template<typename T>
	std::vector<T> ConfigurationLoader::ReadJson(const std::filesystem::path& path) const
	{
		try {
			if (!std::filesystem::exists(path) || std::filesystem::file_size(path) == 0) {
				return {};
			}

			std::ifstream file(path);
			if (!file.is_open()) return {};

			nlohmann::json j;
			file >> j;

			return j.get<std::vector<T>>();
		}
		catch (const std::exception& e) {
			m_logger.LogError(std::format("[Configuration] Failed to load {}: {}", path.filename().string(), e.what()));
			return {};
		}
	}
}