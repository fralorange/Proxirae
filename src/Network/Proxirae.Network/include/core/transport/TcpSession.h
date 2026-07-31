#pragma once

#include <memory>

#include "platform/sock_types.h"
#include "core/io/IIoDriver.h"
#include "diagnostics/ILogger.h"
#include "core/registry/ConnectionEntry.h"
#include "core/proxy/IProxy.h"
#include <core/packet/Endpoint.h>

namespace Proxirae {
	class TcpSession : public std::enable_shared_from_this<TcpSession> {
	public:
		TcpSession(NativeSocket client, Endpoint endpoint, IIoDriver& driver, ILogger& logger);
		~TcpSession();

		std::uint32_t GetAddress() const;
		std::uint16_t GetPort() const;

		void Handle(ConnectionEntry entry, std::function<void(std::shared_ptr<TcpSession>)> onTerminated);
		void Terminate();

	private:
		NativeSocket m_client;
		Endpoint m_endpoint;

		std::unique_ptr<IProxy> m_proxy;

		std::vector<char> m_clientBuffer = std::vector<char>(4096);
		std::vector<char> m_proxyBuffer = std::vector<char>(4096);

		std::function<void(std::shared_ptr<TcpSession>)> m_onTerminated;

		IIoDriver& m_driver;
		ILogger& m_logger;

		std::atomic_bool m_isStopping{ false };

		void StartClientToProxy();
		void StartProxyToClient();
	};
}