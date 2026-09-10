#pragma once

#include <string>

#include "environment/sock_types.h"
#include "proxification/stream/IStreamProxy.h"
#include "asyncio/async/IAsyncDriver.h"
#include "diagnostics/ILogger.h"

namespace Proxirae {
    class Socks5ProxyBase {
    protected:
        std::string m_address;
        std::uint16_t m_port;
        std::string m_username;
        std::string m_password;

        IAsyncDriver& m_driver;
        ILogger& m_logger;

        bool m_connected{ false };

        Socks5ProxyBase(std::string_view address, std::uint16_t port, IAsyncDriver& driver, ILogger& logger);
        Socks5ProxyBase(std::string_view address, std::uint16_t port, std::string_view username, std::string_view password, IAsyncDriver& driver, ILogger& logger);
        virtual ~Socks5ProxyBase() = default;

        virtual NativeSocket ConnectToProxy();
        virtual bool PerformHandshake(NativeSocket sock);

        static bool SendExact(NativeSocket sock, std::span<const char> buffer);
        static bool RecvExact(NativeSocket sock, std::span<char> buffer);
    };
}