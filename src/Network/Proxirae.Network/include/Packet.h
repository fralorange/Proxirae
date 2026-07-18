#pragma once

#include <windivert.h>

namespace Proxirae::Network {
	struct Packet {
		PWINDIVERT_IPHDR ipHdr = nullptr;
		PWINDIVERT_IPV6HDR ipv6Hdr = nullptr;
		UINT8 protocol = 0;
		PWINDIVERT_TCPHDR tcpHdr = nullptr;
		PWINDIVERT_UDPHDR udpHdr = nullptr;
	};
}