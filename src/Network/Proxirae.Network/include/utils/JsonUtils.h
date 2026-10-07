#pragma once

#include <nlohmann/json.hpp>

#include "core/contracts/proxy/ProxyContract.h"
#include "core/contracts/proxy/ProxyTestContract.h"
#include "core/contracts/rule/RuleContract.h"
#include "core/contracts/rule/RuleActionContract.h"
#include "core/contracts/flow/FlowContract.h"
#include "core/contracts/log/LogContract.h"
#include "core/contracts/route/RouteContract.h"
#include "features/persistence/preferences/Preferences.h"
#include "core/contracts/flow/FlowDisconnectContract.h"
#include "core/contracts/flow/FlowDestroyContract.h"
#include "core/contracts/test/TestContract.h"
#include "core/contracts/test/TestProgressContract.h"

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