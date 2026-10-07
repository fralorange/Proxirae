#include "features/proxification/stream/TestableTcpSocks5Proxy.h"

namespace Proxirae {
    NativeSocket TestableTcpSocks5Proxy::ConnectToProxy()
    {
        return MeasureStage(TestStage::Establish, [this] {
            return TcpSocks5Proxy::ConnectToProxy();
        });
    }

    bool TestableTcpSocks5Proxy::PerformHandshake(NativeSocket sock)
    {
        return MeasureStage(TestStage::Handshake, [this, sock] {
            return TcpSocks5Proxy::PerformHandshake(sock);
        });
    }

    bool TestableTcpSocks5Proxy::ConnectToTarget(NativeSocket sock, std::string_view targetAddress, std::uint16_t targetPort)
    {
        return MeasureStage(TestStage::Connect, [this, sock, targetAddress, targetPort] {
            return TcpSocks5Proxy::ConnectToTarget(sock, targetAddress, targetPort);
        });
    }
}