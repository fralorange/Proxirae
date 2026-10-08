#pragma once

#include <string_view>
#include <vector>
#include <memory>

#include "features/proxification/datagram/IDatagramProxy.h"
#include "features/proxification/HttpsProxyBase.h"
#include "asyncio/io/stream/IIoStreamAdapter.h"

namespace Proxirae {
    class UdpHttpsProxy : public IDatagramProxy, protected HttpsProxyBase {
    public:
        UdpHttpsProxy(std::string_view address, std::uint16_t port, IAsyncDriver& driver, IIoStreamAdapter& adapter, ILogger& logger);
        UdpHttpsProxy(std::string_view address, std::uint16_t port, std::string_view username, std::string_view password, IAsyncDriver& driver, IIoStreamAdapter& adapter, ILogger& logger);
        ~UdpHttpsProxy() override;

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

    private:
        NativeSocket m_tcpProxySocket{ InvalidNativeSocket };
        IIoStreamAdapter& m_streamAdapter;

        struct FrameReadContext {
            std::vector<std::byte> headerBuffer;
            std::vector<std::byte> payloadBuffer;
            std::uint16_t payloadLength{ 0 };
        };
        std::shared_ptr<FrameReadContext> m_readCtx;
    };
}