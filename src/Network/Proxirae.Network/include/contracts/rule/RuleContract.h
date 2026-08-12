#pragma once

#include <string>

#include "contracts/rule/RuleProtocol.h"
#include "contracts/rule/RuleActionContract.h"

namespace Proxirae {
	struct RuleContract {
		std::string id;
		int priority;
		bool isEnabled;
		std::string processes;
		std::string hosts;
		std::string ports;
		RuleProtocol protocol;
		RuleActionContract action;

		bool operator<(const RuleContract& other) const;
	};
}