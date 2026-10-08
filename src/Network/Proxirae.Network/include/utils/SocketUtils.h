#pragma once

#include "platform/environment/inet.h"
#include "core/primitives/endpoints/Endpoint.h"
#include "core/primitives/ip/IpAddress.h"

namespace Proxirae::SocketUtils {
	IpAddress FromSockAddr(const sockaddr_in6& addr);
	IpAddress FromSockAddr(const sockaddr_in& addr);
	int ToSockAddr(const Endpoint& endpoint, sockaddr_storage& outStorage, int targetFamily = AF_INET6);
}