#include "features/proxification/stream/TestableTcpHttpsProxy.h"

namespace Proxirae {
    NativeSocket TestableTcpHttpsProxy::ConnectToProxy()
    {
        return MeasureStage(TestStage::Establish, [this] {
            return TcpHttpsProxy::ConnectToProxy();
        });
    }

    bool TestableTcpHttpsProxy::ConnectToTarget(NativeSocket sock, std::string_view targetAddress, std::uint16_t targetPort)
    {
        return MeasureStage(TestStage::Connect, [this, sock, targetAddress, targetPort] {
            return TcpHttpsProxy::ConnectToTarget(sock, targetAddress, targetPort);
        });
    }
}