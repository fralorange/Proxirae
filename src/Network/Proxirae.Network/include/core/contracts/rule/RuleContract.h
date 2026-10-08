#pragma once

#include <string>

#include "core/contracts/rule/RuleProtocol.h"
#include "core/contracts/rule/RuleActionContract.h"

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