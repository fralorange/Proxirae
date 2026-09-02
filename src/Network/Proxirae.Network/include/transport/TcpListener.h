#pragma once

#include "IListener.h"
#include "environment/sock_types.h"
#include "diagnostics/ILogger.h"
#include "transport/TcpSession.h"
#include "asyncio/IIoDriver.h"
#include "proxification/IProxyFactory.h"

namespace Proxirae {
	class TcpListener : public IListener<TcpSession> {
	public:
		TcpListener(IIoDriver& driver, ILogger& logger, IProxyFactory& factory);
		~TcpListener() override;

		std::uint16_t Bind() override;
		bool Listen(std::uint16_t port) override;

		std::shared_ptr<TcpSession> Accept() override;

		void Close() override;

	private:
		NativeSocket m_listener{ InvalidNativeSocket };

		IIoDriver& m_driver;
		ILogger& m_logger;
		IProxyFactory& m_proxyFactory;
	};
}
