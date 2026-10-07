#include <nlohmann/json.hpp>

#include "features/communication/channels/handlers/test/TestPipeHandler.h"
#include "utils/JsonUtils.h"

namespace Proxirae {
	TestPipeHandler::TestPipeHandler(TestController& controller, IProtector& protector)
		: m_controller(controller), m_protector(protector) { }

	void TestPipeHandler::Handle(const PipeMessage& msg)
	{
		nlohmann::json j = nlohmann::json::parse(msg.payload);
		auto test = j.get<TestContract>();

		auto& proxy = test.proxy;

		std::string unprotectedPassword;
		if (m_protector.TryUnprotect(proxy.password, unprotectedPassword))
			proxy.password = unprotectedPassword;

		m_controller.Test(test);
	}
}