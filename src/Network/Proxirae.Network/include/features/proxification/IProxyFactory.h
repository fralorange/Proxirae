#pragma once

#include <memory>
#include <string>

#include "features/proxification/stream/IStreamProxy.h"
#include "features/proxification/datagram/IDatagramProxy.h"

namespace Proxirae {
	class IProxyFactory {
	public:
		virtual ~IProxyFactory() = default;

		virtual std::unique_ptr<IStreamProxy> CreateStream(const std::string proxyId) = 0;
		virtual std::unique_ptr<IDatagramProxy> CreateDatagram(const std::string proxyId) = 0;
	};
}