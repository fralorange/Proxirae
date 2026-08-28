#include <nlohmann/json.hpp>

#include "ipc/TestPipeHandler.h"
#include "utils/JsonUtils.h"

namespace Proxirae {
	TestPipeHandler::TestPipeHandler(TestController& controller)
		: m_controller(controller) { }

	void TestPipeHandler::Handle(const PipeMessage& msg)
	{
		nlohmann::json j = nlohmann::json::parse(msg.payload);
		auto test = j.get<TestContract>();

		m_controller.Test(test);
	}
}