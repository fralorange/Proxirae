#pragma once

#include <string_view>
#include <functional>

#include "proxification/stream/IStreamProxy.h"
#include "proxification/Socks5ProxyBase.h"
#include "asyncio/async/IAsyncDriver.h"
#include "asyncio/io/stream/IIoStreamAdapter.h"

namespace Proxirae {
    class TcpSocks5Proxy : public IStreamProxy, protected Socks5ProxyBase {
    public:
        TcpSocks5Proxy(std::string_view address, std::uint16_t port, IAsyncDriver& driver, IIoStreamAdapter& adapter, ILogger& logger);
        TcpSocks5Proxy(std::string_view address, std::uint16_t port, std::string_view username, std::string_view password, IAsyncDriver& driver, IIoStreamAdapter& adapter, ILogger& logger);

        ~TcpSocks5Proxy() override;

        bool Connect(std::string_view targetAddress, std::uint16_t targetPort) override;
        void Disconnect() override;

        void Send(std::span<const std::byte> buffer, std::function<void(const IoResult&)> callback) override;
        void Recv(std::span<std::byte> buffer, std::function<void(const IoResult&)> callback) override;

    protected:
        virtual bool ConnectToTarget(NativeSocket sock, std::string_view targetAddress, std::uint16_t targetPort);

    private:
        NativeSocket m_proxy{ InvalidNativeSocket };

        IIoStreamAdapter& m_adapter;
    };
}