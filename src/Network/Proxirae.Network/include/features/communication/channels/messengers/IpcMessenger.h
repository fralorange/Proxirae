#pragma once

#include "features/communication/channels/ISendChannel.h"

namespace Proxirae {
	class IpcMessenger {
	public:
		explicit IpcMessenger(ISendChannel& sender);

		template<typename T>
		bool Send(PipeMessageType type, const T& payload);
			
	private:
		ISendChannel& m_sender;
	};
}

#include "features/communication/channels/messengers/IpcMessenger.inl"