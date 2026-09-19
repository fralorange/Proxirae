#include "proxification/ProxyFactory.h"
#include "proxification/stream/TcpSocks5Proxy.h"
#include "proxification/datagram/UdpSocks5Proxy.h"

namespace Proxirae {
	ProxyFactory::ProxyFactory(Store<Configuration>& config, IAsyncDriver& driver, IIoStreamAdapter& streamAdapter, IIoDatagramAdapter& datagramAdapter, ILogger& logger)
		: m_config(config), m_driver(driver), m_streamAdapter(streamAdapter), m_datagramAdapter(datagramAdapter), m_logger(logger) { }
	
	std::unique_ptr<IStreamProxy> ProxyFactory::CreateStream(const std::string proxyId)
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
			case ProxyType::SOCKS5:
				return std::make_unique<TcpSocks5Proxy>(contract.address, contract.port, contract.username, contract.password, m_driver, m_streamAdapter, m_logger);
		}

		return nullptr;
	}

	std::unique_ptr<IDatagramProxy> ProxyFactory::CreateDatagram(const std::string proxyId)
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
		case ProxyType::SOCKS5:
			return std::make_unique<UdpSocks5Proxy>(
				contract.address, contract.port, contract.username, contract.password, m_driver, m_datagramAdapter, m_logger);
		}

		return nullptr;
	}
}