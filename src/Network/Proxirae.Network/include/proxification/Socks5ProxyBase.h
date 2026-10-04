#pragma once

#include "proxification/ProxyBase.h"

namespace Proxirae {
    class Socks5ProxyBase : public ProxyBase {
    protected:
        using ProxyBase::ProxyBase; 

        virtual bool PerformHandshake(NativeSocket sock);
    };
}