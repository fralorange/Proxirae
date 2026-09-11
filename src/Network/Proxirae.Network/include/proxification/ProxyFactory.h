#pragma once

#include <string>
#include <memory>

#include "proxification/IProxyFactory.h"
#include "persistence/Store.h"
#include "persistence/configuration/Configuration.h"
#include "asyncio/async/IAsyncDriver.h"
#include "asyncio/io/stream/IIoStreamAdapter.h"
#include "asyncio/io/datagram/IIoDatagramAdapter.h"
#include "diagnostics/ILogger.h"

namespace Proxirae {
	class ProxyFactory : public IProxyFactory {
	public:
		ProxyFactory(Store<Configuration>& config, IAsyncDriver& driver, IIoStreamAdapter& streamAdapter, IIoDatagramAdapter& datagramAdapter, ILogger& logger);

		std::unique_ptr<IStreamProxy> CreateStream(const std::string proxyId) override;
		std::unique_ptr<IDatagramProxy> CreateDatagram(const std::string proxyId) override;

	private:
		Store<Configuration>& m_config;

		IAsyncDriver& m_driver;
		IIoStreamAdapter& m_streamAdapter;
		IIoDatagramAdapter& m_datagramAdapter;

		ILogger& m_logger;
	};
}