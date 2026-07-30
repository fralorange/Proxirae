#pragma once

#include <optional>
#include <WinSock2.h>

#include "diagnostics/ILogger.h"
#include "registry/ConnectionEntry.h"

namespace Proxirae {
	class TcpSession {
	public:
		TcpSession(SOCKET client, SOCKADDR_IN address, ILogger& logger);
		~TcpSession();

		UINT32 GetAddress() const;
		UINT16 GetPort() const;

		void Handle(ConnectionEntry entry);
		void Stop();

	private:
		SOCKET m_client;
		SOCKADDR_IN m_address;

		ILogger& m_logger;

		std::atomic_bool m_isStopping{ false };
	};
}