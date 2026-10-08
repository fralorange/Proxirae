#pragma once

#include <string>
#include <string_view>
#include <cstdint>
#include <span>

#include "platform/environment/sock.h"
#include "features/diagnostics/ILogger.h"
#include "asyncio/async/IAsyncDriver.h"

namespace Proxirae {
    class ProxyBase {
    public:
        ProxyBase(std::string_view address, std::uint16_t port, IAsyncDriver& driver, ILogger& logger);
        ProxyBase(std::string_view address, std::uint16_t port, std::string_view username, std::string_view password, IAsyncDriver& driver, ILogger& logger);
        virtual ~ProxyBase() = default;

    protected:
        virtual bool PerformHandshake(NativeSocket sock);

        virtual NativeSocket ConnectToProxy();
        NativeSocket ResolveDomain();

        static bool SendExact(NativeSocket sock, std::span<const char> buffer);
        static bool RecvExact(NativeSocket sock, std::span<char> buffer);

        std::string m_address;
        std::uint16_t m_port;
        std::string m_username;
        std::string m_password;
        IAsyncDriver& m_driver;
        ILogger& m_logger;

		bool m_connected{ false };
    };
}