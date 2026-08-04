#pragma once

#include <vector>

#include "core/packet/PacketMetadata.h"

namespace Proxirae {
	struct Packet {
		std::vector<std::uint8_t> data;
		std::uint32_t length;
		PacketMetadata metadata;
	};
}