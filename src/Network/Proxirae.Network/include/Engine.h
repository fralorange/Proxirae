#pragma once

#include <windivert.h>

#include "Persistence/ConnectionTable.h"
#include "Logging/ILogger.h"

namespace Proxirae::Network {
	class Engine {
	public:
		Engine(Persistence::ConnectionTable& connections, Logging::ILogger& logger);
		~Engine();

		void Start();
		void Stop();

	private:
		HANDLE m_handle;

		Persistence::ConnectionTable& m_connections;

		std::vector<unsigned char> m_packetBuffer;
		UINT m_recvLen;
		WINDIVERT_ADDRESS m_addr;

		Logging::ILogger& m_logger;

		bool m_running;
	};
}
