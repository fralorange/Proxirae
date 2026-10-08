#pragma once

#include "features/communication/channels/messages/PipeMessage.h"

namespace Proxirae {
	class ISendChannel {
	public:
		virtual ~ISendChannel() = default;

		virtual bool Send(const PipeMessage& msg) = 0;
	};
}