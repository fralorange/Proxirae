#pragma once
#include "communication/channels/messages/PipeMessage.h"

namespace Proxirae {
	class IPipeMessageHandler {
	public:
		virtual ~IPipeMessageHandler() = default;

		virtual void Handle(const PipeMessage& msg) = 0;
	};
}