#pragma once
#include <cstdint>

#if defined(__linux__)
    #include <sys/socket.h>
#endif

namespace Proxirae {
#ifdef _WIN32
    using NativeSocket = std::uintptr_t;
    constexpr NativeSocket InvalidNativeSocket = ~0;
    using NativeSocketLen = int;
#elif defined(__linux__)
    using NativeSocket = int;
    constexpr NativeSocket InvalidNativeSocket = -1;
    using NativeSocketLen = socklen_t;
#else 
    #error "Unsupported platform"
#endif 
}