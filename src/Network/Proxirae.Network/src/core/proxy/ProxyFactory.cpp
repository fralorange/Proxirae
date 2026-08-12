#include "core/proxy/ProxyFactory.h"
#include "core/proxy/Socks5Proxy.h"

namespace Proxirae {
	ProxyFactory::ProxyFactory(std::unordered_map<std::string, ProxyContract>& proxies, IIoDriver& driver, ILogger& logger) 
		: m_proxies(proxies), m_driver(driver), m_logger(logger) { }
	
	std::unique_ptr<IProxy> ProxyFactory::Create(const std::string proxyId)
	{
		auto it = m_proxies.find(proxyId);

		if (it == m_proxies.end()) {
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