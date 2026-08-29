#include "utils/UuidUtils.h"

namespace Proxirae::UuidUtils {
	std::string GenerateUUID()
    {
#ifdef WIN32
        UUID uuid;
        if (UuidCreate(&uuid) != RPC_S_OK) {
            return std::string();
        }

        unsigned char* str;
        if (UuidToStringA(&uuid, &str) != RPC_S_OK) {
            return std::string();
        }

        std::string s((char*)str);

        RpcStringFreeA(&str);
#else
        uuid_t uuid;
        uuid_generate_random(uuid);
        char s[37];
        uuid_unparse(uuid, s);
#endif
        return s;
    }
}