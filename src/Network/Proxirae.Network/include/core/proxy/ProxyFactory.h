#pragma once

#include <unordered_map>
#include <string>

#include "core/proxy/IProxyFactory.h"
#include "contracts/proxy/ProxyContract.h"
#include "core/io/IIoDriver.h"
#include "diagnostics/ILogger.h"

namespace Proxirae {
	class ProxyFactory : public IProxyFactory {
	public:
		ProxyFactory(std::unordered_map<std::string, ProxyContract>& proxies, IIoDriver& driver, ILogger& logger);

		std::unique_ptr<IProxy> Create(const std::string proxyId) override;

	private:
		std::unordered_map<std::string, ProxyContract>& m_proxies;
		IIoDriver& m_driver;
		ILogger& m_logger;
	};
}