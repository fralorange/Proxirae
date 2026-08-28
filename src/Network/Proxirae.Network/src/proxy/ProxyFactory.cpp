#include "proxy/ProxyFactory.h"
#include "proxy/Socks5Proxy.h"

namespace Proxirae {
	ProxyFactory::ProxyFactory(Store<Configuration>& config, IIoDriver& driver, ILogger& logger)
		: m_config(config), m_driver(driver), m_logger(logger) { }
	
	std::unique_ptr<IProxy> ProxyFactory::Create(const std::string proxyId)
	{
		auto configuration = m_config.Get();

		auto it = configuration->proxies.find(proxyId);

		if (it == configuration->proxies.end()) {
			return nullptr;
		}

		const auto& contract = it->second;

		switch (contract.type) {
			case ProxyType::HTTPS:
				// No implementation, yet
				break;
			case ProxyType::SOCKS4:
				// No implementation, yet
				break;
			case ProxyType::SOCKS5:
				return std::make_unique<Socks5Proxy>(contract.address, contract.port, contract.username, contract.password, m_driver, m_logger);
		}

		return nullptr;
	}
}