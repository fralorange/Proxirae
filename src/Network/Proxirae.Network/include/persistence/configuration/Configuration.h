#pragma once
#include <unordered_map>
#include <set>
#include <string>

#include "contracts/proxy/ProxyContract.h"
#include "contracts/rule/RuleContract.h"

namespace Proxirae {
	struct Configuration {
		std::unordered_map<std::string, ProxyContract> proxies;
		std::set<RuleContract> rules;
	};
}