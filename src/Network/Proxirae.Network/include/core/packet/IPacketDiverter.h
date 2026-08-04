#pragma once

#include <functional>

#include "core/packet/IPacketContext.h"

namespace Proxirae {
	class IPacketDiverter {
	public:
		virtual ~IPacketDiverter() = default;

		virtual bool Open() = 0;
		virtual void Close() = 0;

		virtual bool Receive(const std::function<void(IPacketContext&)>& callback) = 0; 

		virtual bool Send(IPacketContext& ctx) = 0;
	};
}