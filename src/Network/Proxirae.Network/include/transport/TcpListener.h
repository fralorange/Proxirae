#pragma once

#include "environment/sock_types.h"
#include "diagnostics/ILogger.h"
#include "transport/TcpSession.h"
#include "asyncio/IIoDriver.h"
#include "proxification/IProxyFactory.h"

namespace Proxirae {
	class TcpListener {
	public:
		TcpListener(IIoDriver& driver, ILogger& logger, IProxyFactory& factory);
		~TcpListener();

		std::uint16_t Bind();
		bool Listen(std::uint16_t port);

		std::shared_ptr<TcpSession> Accept();

		void Close();

	private:
		NativeSocket m_listener{ InvalidNativeSocket };

		IIoDriver& m_driver;
		ILogger& m_logger;
		IProxyFactory& m_proxyFactory;
	};
}
