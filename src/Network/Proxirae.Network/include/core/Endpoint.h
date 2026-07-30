#pragma once

#include <basetsd.h>
#include <string>

namespace Proxirae {
	class Endpoint {
	public:
		Endpoint(UINT32 address, UINT16 port);
		
		std::string ToString() const;
	private:
		UINT32 m_address;
		UINT16 m_port;
	};
}