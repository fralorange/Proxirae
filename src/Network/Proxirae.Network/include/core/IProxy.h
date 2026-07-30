#pragma once

#include <string_view>
#include <basetsd.h>

namespace Proxirae {
	class IProxy {
	public:
		virtual ~IProxy() = default;

		virtual bool Connect(std::string_view targetAddress, UINT16 port) = 0;
		virtual void Disconnect() = 0;

		virtual int Send(const char* buffer, int length) = 0;
		virtual int Recv(char* buffer, int length) = 0;
	};
}