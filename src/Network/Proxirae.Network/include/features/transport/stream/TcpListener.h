#pragma once

#include "platform/environment/sock_types.h"
#include "features/diagnostics/ILogger.h"
#include "features/transport/stream/TcpSession.h"
#include "asyncio/async/IAsyncDriver.h"
#include "features/proxification/IProxyFactory.h"
#include "asyncio/io/stream/IIoStreamAdapter.h"

namespace Proxirae {
	class TcpListener {
	public:
		TcpListener(IAsyncDriver& driver, IIoStreamAdapter& adapter, ILogger& logger, IProxyFactory& factory);
		~TcpListener();

		std::uint16_t Bind();
		bool Listen(std::uint16_t port);

		std::shared_ptr<TcpSession> Accept();

		void Close();
	private:
		NativeSocket m_listener{ InvalidNativeSocket };

		IAsyncDriver& m_driver;
		IIoStreamAdapter& m_adapter;
		ILogger& m_logger;
		IProxyFactory& m_proxyFactory;
	};
}
