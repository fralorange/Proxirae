#pragma once
#include "Packet.h"

namespace Proxirae {
	class IPacketHandler {
	public:
		virtual ~IPacketHandler() = default;

		virtual bool CanHandle(const Packet& packet) = 0;
		virtual void Handle(Packet& packet) = 0;
	};
}