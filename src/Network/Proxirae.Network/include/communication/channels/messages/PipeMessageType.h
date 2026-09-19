#pragma once

#include <cstdint>

namespace Proxirae {
	enum class PipeMessageType : std::uint16_t {
		Cmd_ReloadProxies = 1,
		Cmd_ReloadRules,
		Cmd_ReloadPreferences,
		Cmd_DisconnectFlow,
		Cmd_EndFlowProcess,
		Cmd_CheckProxy,

		Evt_SendFlow = 1000,
		Evt_SendLog,
		Evt_SendRoute,
		Evt_SendTestProgress
	};
}
