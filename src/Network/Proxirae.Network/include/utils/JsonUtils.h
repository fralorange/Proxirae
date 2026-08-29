#pragma once

#include <nlohmann/json.hpp>

#include "contracts/proxy/ProxyContract.h"
#include "contracts/proxy/ProxyTestContract.h"
#include "contracts/rule/RuleContract.h"
#include "contracts/rule/RuleActionContract.h"
#include "contracts/flow/FlowContract.h"
#include "contracts/log/LogContract.h"
#include "contracts/route/RouteContract.h"
#include "persistence/Preferences.h"
#include "contracts/flow/FlowDisconnectContract.h"
#include "contracts/flow/FlowDestroyContract.h"
#include "contracts/test/TestContract.h"
#include "contracts/test/TestProgressContract.h"

namespace Proxirae {
	void from_json(const nlohmann::json& j, ProxyContract& p);
	void from_json(const nlohmann::json& j, ProxyTestContract& p);
	void from_json(const nlohmann::json& j, RuleContract& r);
	void from_json(const nlohmann::json& j, RuleActionContract& a);
	void from_json(const nlohmann::json& j, Preferences& p);
	void from_json(const nlohmann::json& j, FlowDisconnectContract& f);
	void from_json(const nlohmann::json& j, FlowDestroyContract& f);
	void from_json(const nlohmann::json& j, TestContract& t);

	void to_json(nlohmann::json& j, const FlowContract& f);
	void to_json(nlohmann::json& j, const LogContract& l);
	void to_json(nlohmann::json& j, const RouteContract& r);
	void to_json(nlohmann::json& j, const TestProgressContract& t);
}