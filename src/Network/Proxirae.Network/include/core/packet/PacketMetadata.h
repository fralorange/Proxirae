#pragma once

#if defined(_WIN32)
	#include <windivert.h>
	namespace Proxirae {
		using PacketMetadata = WINDIVERT_ADDRESS;
	}
#elif defined(__linux__)
	#include <stdint.h>
	namespace Proxirae {
		using PacketMetadata = uint32_t; // packetId
	}
#else
	#error "Unsupported platform"
#endif