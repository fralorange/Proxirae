#include <nlohmann/json.hpp>

#include "ipc/FlowPipeHandler.h"
#include "utils/JsonUtils.h"

namespace Proxirae {
	FlowPipeHandler::FlowPipeHandler(SessionController& controller)
		: m_controller(controller) {}

	void FlowPipeHandler::Handle(const PipeMessage& msg)
	{
		nlohmann::json j = nlohmann::json::parse(msg.payload);
		
		if (msg.header.type == PipeMessageType::Cmd_DisconnectFlow) {
			auto disconnectFlow = j.get<FlowDisconnectContract>();

			m_controller.DropSession(disconnectFlow.id);
		}
		else if (msg.header.type == PipeMessageType::Cmd_EndFlowProcess) {
			auto destroyFlow = j.get<FlowDestroyContract>();

			m_controller.KillSessionProcess(destroyFlow.processId);
		}
	}
}