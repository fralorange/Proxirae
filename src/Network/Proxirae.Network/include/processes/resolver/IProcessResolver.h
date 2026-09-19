#pragma once

#include <optional>

#include "processes/resolver/ProcessInfo.h"

namespace Proxirae {
	class IProcessResolver {
	public:
		virtual ~IProcessResolver() = default;

		virtual std::optional<ProcessInfo> Resolve(std::uint32_t pid) = 0;
	};
}