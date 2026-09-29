#pragma once

#include "environment/inet.h"
#include "primitives/endpoints/Endpoint.h"
#include "primitives/ip/IpAddress.h"

namespace Proxirae::SocketUtils {
	IpAddress FromSockAddr(const sockaddr_in6& addr);
	IpAddress FromSockAddr(const sockaddr_in& addr);
	int ToSockAddr(const Endpoint& endpoint, sockaddr_storage& outStorage);
}