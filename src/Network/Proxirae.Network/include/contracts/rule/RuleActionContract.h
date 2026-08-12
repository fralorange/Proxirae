#pragma once

#include <optional>
#include <string>

#include "contracts/rule/RuleAction.h"

namespace Proxirae {
	struct RuleActionContract {
		RuleAction action;
		std::optional<std::string> proxyId;
	};
}