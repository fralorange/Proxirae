#include "persistence/ConfigurationStore.h"

namespace Proxirae {
	ConfigurationStore::ConfigurationStore(ConfigurationPtr configuration)
		: m_current(std::move(configuration)) { }

	ConfigurationPtr ConfigurationStore::Get() const {
		return m_current.load();
	}

	void ConfigurationStore::Update(ConfigurationPtr configuration)
	{
		m_current.store(std::move(configuration));
	}
}