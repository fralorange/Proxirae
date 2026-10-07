#pragma once

#include "features/proxification/ProxyBase.h"

namespace Proxirae {
    class Socks5ProxyBase : public ProxyBase {
    protected:
        using ProxyBase::ProxyBase;

        bool PerformHandshake(NativeSocket sock) override;
    };
}