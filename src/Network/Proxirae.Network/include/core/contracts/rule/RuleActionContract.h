#pragma once

#include <optional>
#include <string>

#include "core/contracts/rule/RuleAction.h"

namespace Proxirae {
	struct RuleActionContract {
		RuleAction action;
		std::optional<std::string> proxyId;
	};
}