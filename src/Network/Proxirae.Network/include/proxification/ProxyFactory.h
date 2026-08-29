#pragma once

#include <string>

#include "proxification/IProxyFactory.h"
#include "persistence/Store.h"
#include "persistence/Configuration.h"
#include "asyncio/IIoDriver.h"
#include "diagnostics/ILogger.h"

namespace Proxirae {
	class ProxyFactory : public IProxyFactory {
	public:
		ProxyFactory(Store<Configuration>& config, IIoDriver& driver, ILogger& logger);

		std::unique_ptr<IProxy> Create(const std::string proxyId) override;

	private:
		Store<Configuration>& m_config;
		IIoDriver& m_driver;
		ILogger& m_logger;
	};
}