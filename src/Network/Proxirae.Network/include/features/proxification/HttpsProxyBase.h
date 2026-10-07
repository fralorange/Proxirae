#pragma once

#include "features/proxification/ProxyBase.h"

namespace Proxirae {
    class HttpsProxyBase : public ProxyBase {
    protected:
        using ProxyBase::ProxyBase; 

        bool ReadHttpHeaders(NativeSocket sock, std::string& outHeaders);
        static std::string Base64Encode(std::string_view input);
    };
}