#pragma once

#include <cstdint>
#include "PipeMessageType.h"

namespace Proxirae {
#pragma pack(push, 1)
	struct PipeMessageHeader {
		std::uint16_t magic{ 0x5850 }; // PX
		PipeMessageType type{};
		std::uint32_t id{ 0 };
		std::uint32_t payloadSize{ 0 };
	};
#pragma pack(pop)
}