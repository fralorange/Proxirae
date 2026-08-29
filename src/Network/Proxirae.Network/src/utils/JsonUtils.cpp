#include "utils/JsonUtils.h"

namespace Proxirae {
	void from_json(const nlohmann::json& j, ProxyContract& p)
	{
		ProxyTestContract base = j.get<ProxyTestContract>();

		p.address = std::move(base.address);
		p.port = base.port;
		p.type = base.type;
		p.username = std::move(base.username);
		p.password = std::move(base.password);

		j.at("Id").get_to(p.id);
	}

	void from_json(const nlohmann::json& j, ProxyTestContract& p)
	{
		j.at("Address").get_to(p.address);
		j.at("Port").get_to(p.port);

		p.type = static_cast<ProxyType>(j.at("Type").get<int>());

		p.username = j.contains("Username") && !j["Username"].is_null() ? j["Username"].get<std::string>() : "";
		p.password = j.contains("Password") && !j["Password"].is_null() ? j["Password"].get<std::string>() : "";
	}

	void from_json(const nlohmann::json& j, RuleContract& r)
	{
		j.at("Id").get_to(r.id);

		r.isEnabled = j.value("IsEnabled", true);
		r.priority = j.value("Priority", 0);

		r.processes = j.contains("Processes") && !j["Processes"].is_null() ? j["Processes"].get<std::string>() : "";
		r.hosts = j.contains("Hosts") && !j["Hosts"].is_null() ? j["Hosts"].get<std::string>() : "";
		r.ports = j.contains("Ports") && !j["Ports"].is_null() ? j["Ports"].get<std::string>() : "";

		r.protocol = static_cast<RuleProtocol>(j.value("Protocol", 0));

		j.at("Action").get_to(r.action);
	}

	void from_json(const nlohmann::json& j, RuleActionContract& a)
	{
		if (j.contains("ProxyId") && !j["ProxyId"].is_null()) {
			a.proxyId = j["ProxyId"].get<std::string>();
			a.action = RuleAction::Proxy;
		}
		else {
			a.proxyId = std::nullopt;

			std::string name = j.value("Name", "");
			if (name.find("Block") != std::string::npos) {
				a.action = RuleAction::Block;
			}
			else {
				a.action = RuleAction::Direct;
			}
		}
	}

	void from_json(const nlohmann::json& j, Preferences& p)
	{
		p.logLevel = j.value("LogLevel", p.logLevel);
	}

	void from_json(const nlohmann::json& j, FlowDisconnectContract& f)
	{
		f.id = j.value("Id", f.id);
	}

	void from_json(const nlohmann::json& j, FlowDestroyContract& f)
	{
		f.processId = j.value("ProcessId", f.processId);
	}

	void from_json(const nlohmann::json& j, TestContract& t)
	{
		j.at("Id").get_to(t.id);
		j.at("Proxy").get_to(t.proxy);
		j.at("TestAddress").get_to(t.testAddress);
		j.at("TestPort").get_to(t.testPort);
	}

	void to_json(nlohmann::json& j, const FlowContract& f)
	{
		j = nlohmann::json{
			{"Id", f.id},
			{"TargetAddress", f.targetAddress},
			{"TargetPort", f.targetPort},
			{"ProcessId", f.processId},
			{"SecondsPassed", f.secondsPassed},
			{"ProxyId", f.proxyId},
			{"BytesSent", f.bytesSent},
			{"BytesReceived", f.bytesReceived},
			{"Status", f.status} 
		};
	}

	void to_json(nlohmann::json& j, const LogContract& l)
	{
		auto isoTime = std::format("{:%FT%TZ}", l.timestamp);

		j = nlohmann::json{
			{"Level", l.level},
			{"Message", l.message},
			{"Timestamp", isoTime}
		};
	}

	void to_json(nlohmann::json& j, const RouteContract& r)
	{
		auto isoTime = std::format("{:%FT%TZ}", r.timestamp);

		j = nlohmann::json{
			{"Timestamp", isoTime},
			{"ProcessId", r.processId},
			{"Address", r.address},
			{"Port", r.port},
			{"RuleId", r.ruleId}
		};
	}

	void to_json(nlohmann::json& j, const TestProgressContract& t)
	{
		j = nlohmann::json{
			{"Id", t.id},
			{"Stage", t.stage},
			{"Latency", t.latency}
		};
	}
}