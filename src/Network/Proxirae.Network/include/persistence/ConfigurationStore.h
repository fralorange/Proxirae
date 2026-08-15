#pragma once
#include <memory>

#include "Configuration.h"

namespace Proxirae {
	using ConfigurationPtr = std::shared_ptr<const Configuration>;

	class ConfigurationStore {
	public:
		explicit ConfigurationStore(ConfigurationPtr configuration);

		ConfigurationPtr Get() const;
		void Update(ConfigurationPtr configuration);

	private:
		std::atomic<ConfigurationPtr> m_current;
	};
}