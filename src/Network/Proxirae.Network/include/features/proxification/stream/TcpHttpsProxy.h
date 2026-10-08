#pragma once

#include <string_view>

#include "features/proxification/stream/IStreamProxy.h"
#include "features/proxification/HttpsProxyBase.h"
#include "asyncio/io/stream/IIoStreamAdapter.h"

namespace Proxirae {
    class TcpHttpsProxy : public IStreamProxy, protected HttpsProxyBase {
    public:
        TcpHttpsProxy(std::string_view address, std::uint16_t port, IAsyncDriver& driver, IIoStreamAdapter& adapter, ILogger& logger);
        TcpHttpsProxy(std::string_view address, std::uint16_t port, std::string_view username, std::string_view password, IAsyncDriver& driver, IIoStreamAdapter& adapter, ILogger& logger);
        ~TcpHttpsProxy() override;

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