#pragma once

#include <cstdint>

#include "environment/sock_types.h"
#include "diagnostics/ILogger.h"
#include "asyncio/async/IAsyncDriver.h"

namespace Proxirae {
	class UdpBinder {
    public:
        UdpBinder(IAsyncDriver& driver, ILogger& logger);
        ~UdpBinder();

        std::uint16_t Bind(std::uint16_t requestedPort = 0);

        NativeSocket ReleaseSocket();

    private:
        NativeSocket m_socket{ InvalidNativeSocket };

        IAsyncDriver& m_driver;
        ILogger& m_logger;
	};
}