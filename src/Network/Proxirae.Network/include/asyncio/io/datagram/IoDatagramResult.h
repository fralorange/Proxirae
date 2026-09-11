#pragma once

#include <WinSock2.h>
#include <cstddef>

#include "asyncio/io/IoResult.h"

namespace Proxirae {
	struct IoDatagramResult : public IoResult {
		sockaddr_storage remoteAddr{};
		int remoteAddrLen{ sizeof(sockaddr_storage) };
	};
}