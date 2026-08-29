#pragma once
#include "IPipeMessageHandler.h"
#include "communication/PipeMessageType.h"

namespace Proxirae {
	class IDispatchChannel {
	public:
		virtual ~IDispatchChannel() = default;

		virtual void Dispatch(const PipeMessage& message) = 0;
		virtual void RegisterHandler(PipeMessageType messageType, IPipeMessageHandler& handler) = 0;
	};
}