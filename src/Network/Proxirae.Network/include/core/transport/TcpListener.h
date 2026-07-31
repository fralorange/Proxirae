#pragma once

#include "platform/sock_types.h"
#include "diagnostics/ILogger.h"
#include "core/transport/TcpSession.h"
#include "core/io/IIoDriver.h"

namespace Proxirae {
	class TcpListener {
	public:
		TcpListener(IIoDriver& driver, ILogger& logger);
		~TcpListener();

		std::uint16_t Bind();
		bool Listen(std::uint16_t port);
		std::shared_ptr<TcpSession> Accept();

	private:
		NativeSocket m_listener{ InvalidNativeSocket };

		IIoDriver& m_driver;
		ILogger& m_logger;
	};
}
