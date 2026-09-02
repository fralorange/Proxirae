#pragma once

#include <string>
#include <memory>

#include "persistence/Configuration.h"

namespace Proxirae {
	class PacketFilter {
	public:
		static std::string BuildNetworkFilter(std::shared_ptr<const Configuration> config);
		static std::string BuildSocketFilter();
	};
}