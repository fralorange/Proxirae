#pragma once

#include <vector>

#include "interception/PacketMetadata.h"

namespace Proxirae {
	struct Packet {
		std::vector<std::uint8_t> data;
		std::uint32_t length;
		PacketMetadata metadata;
	};
}