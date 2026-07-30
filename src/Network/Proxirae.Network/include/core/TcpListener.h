#pragma once

#include <WinSock2.h>

#include "diagnostics/ILogger.h"
#include "core/TcpSession.h"

namespace Proxirae {
	class TcpListener {
	public:
		explicit TcpListener(ILogger& logger);
		~TcpListener();

		bool Listen(UINT16 port);
		std::shared_ptr<TcpSession> Accept();

	private:
		SOCKET m_listener{ INVALID_SOCKET };

		ILogger& m_logger;
	};
}
