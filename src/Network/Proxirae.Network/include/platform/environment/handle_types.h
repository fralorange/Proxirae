#pragma once

#ifdef _WIN32
#include <cstdint>
#endif 

namespace Proxirae {
#ifdef _WIN32
	using NativeHandle = std::uintptr_t;
#elif defined(__linux__)
	using NativeHandle = int;
#else 
	#error Unsupported platform
#endif 
}