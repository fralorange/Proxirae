#pragma once

#include "communication/ISendChannel.h"

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

#include "communication/IpcMessenger.inl"