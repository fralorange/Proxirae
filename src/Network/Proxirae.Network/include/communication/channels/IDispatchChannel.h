#pragma once
#include "communication/channels/handlers/IPipeMessageHandler.h"
#include "communication/channels/messages/PipeMessageType.h"

namespace Proxirae {
	class IDispatchChannel {
	public:
		virtual ~IDispatchChannel() = default;

		virtual void Dispatch(const PipeMessage& message) = 0;
		virtual void RegisterHandler(PipeMessageType messageType, IPipeMessageHandler& handler) = 0;
	};
}