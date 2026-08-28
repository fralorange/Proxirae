#pragma once

#include <stop_token>

#include "ipc/IReceiveChannel.h"
#include "ipc/IDispatchChannel.h"

namespace Proxirae {
	class Host {
	public:
		Host(IReceiveChannel& receiver, IDispatchChannel& dispatcher, std::stop_source source);

		void Run();
		
	private:
		IReceiveChannel& m_receiver;
		IDispatchChannel& m_dispatcher;
		std::stop_source m_source;
	};
}