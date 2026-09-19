#pragma once

#include <string_view>
#include <vector>
#include <functional>

#include "proxification/datagram/IDatagramProxy.h"
#include "proxification/Socks5ProxyBase.h"
#include "asyncio/io/datagram/IIoDatagramAdapter.h"

namespace Proxirae {
    class UdpSocks5Proxy : public IDatagramProxy, protected Socks5ProxyBase {
    public:
        UdpSocks5Proxy(std::string_view address, std::uint16_t port, IAsyncDriver& driver, IIoDatagramAdapter& adapter, ILogger& logger);
        UdpSocks5Proxy(std::string_view address, std::uint16_t port, std::string_view username, std::string_view password, IAsyncDriver& driver, IIoDatagramAdapter& adapter, ILogger& logger);

        ~UdpSocks5Proxy() override;

        bool Connect() override;
        void Disconnect() override;

        void Send(
            std::span<const std::byte> buffer,
            std::string_view targetAddress,
            std::uint16_t targetPort,
            std::function<void(const IoResult&)> callback
        ) override;

        void Recv(
            std::span<std::byte> buffer,
            std::function<void(const IoResult& result, std::string sourceAddress, std::uint16_t sourcePort)> callback
        ) override;

    protected:
        bool RequestUdpAssociate(NativeSocket sock);
        NativeSocket ConnectToRelay(NativeSocket sock);

    private:
        NativeSocket m_tcpControl{ InvalidNativeSocket };
        NativeSocket m_udpData{ InvalidNativeSocket };

        std::string m_bindAddress;
        std::uint16_t m_bindPort{ 0 };

        std::vector<std::byte> m_internalRecvBuffer;

        IIoDatagramAdapter& m_adapter;
    };
}