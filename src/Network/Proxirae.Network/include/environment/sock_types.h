#pragma once
#include <cstdint>

namespace Proxirae {
#ifdef _WIN32
	using NativeSocket = std::uintptr_t;
	constexpr NativeSocket InvalidNativeSocket = ~0;
#elif defined(__linux__)
	using NativeSocket = int;
	constexpr NativeSocket InvalidNativeSocket = -1;
#else 
	#error Unsupported platform
#endif 
}