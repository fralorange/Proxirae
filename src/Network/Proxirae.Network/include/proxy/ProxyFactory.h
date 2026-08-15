#pragma once

#include <string>

#include "proxy/IProxyFactory.h"
#include "persistence/ConfigurationStore.h"
#include "io/IIoDriver.h"
#include "diagnostics/ILogger.h"

namespace Proxirae {
	class ProxyFactory : public IProxyFactory {
	public:
		ProxyFactory(ConfigurationStore& config, IIoDriver& driver, ILogger& logger);

		std::unique_ptr<IProxy> Create(const std::string proxyId) override;

	private:
		ConfigurationStore& m_config;
		IIoDriver& m_driver;
		ILogger& m_logger;
	};
}