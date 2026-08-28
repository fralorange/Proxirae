#pragma once

#include <cstddef>
#include <span>
#include <string>

#include <nlohmann/json.hpp>

#include "ipc/PipeMessage.h"
#include "utils/JsonUtils.h"

namespace Proxirae {
	template <typename T>
	bool IpcMessenger::Send(PipeMessageType type, const T& payload)
	{
		nlohmann::json j = payload;
		std::string jsonStr = j.dump();

		auto bytes = std::vector<std::byte>(
			reinterpret_cast<const std::byte*>(jsonStr.data()),
			reinterpret_cast<const std::byte*>(jsonStr.data()) + jsonStr.size()
		);

		auto msg = PipeMessage::Create(type, bytes);

		return m_sender.Send(msg);
	}
}