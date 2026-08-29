#pragma once

#include <cstdint>
#include <string>

#include "FlowStatus.h"

namespace Proxirae {
	struct FlowContract {
		std::string id;
		std::uint32_t targetAddress;
		std::uint16_t targetPort;
		std::int64_t processId;
		std::uint64_t secondsPassed;
		std::string proxyId;
		std::uint64_t bytesSent;
		std::uint64_t bytesReceived;
		FlowStatus status;
	};
}