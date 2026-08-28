#include <string>

#ifdef _WIN32
	#pragma comment(lib, "Rpcrt4.lib")
	#include <Rpc.h>
#else
	#include <uuid/uuid.h>
#endif

namespace Proxirae::UuidUtils {
	std::string GenerateUUID();
}