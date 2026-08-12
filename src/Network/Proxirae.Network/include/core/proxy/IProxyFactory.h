#pragma once

#include <memory>
#include <string>

#include "core/proxy/IProxy.h"

namespace Proxirae {
	class IProxyFactory {
	public:
		virtual ~IProxyFactory() = default;

		virtual std::unique_ptr<IProxy> Create(const std::string proxyId) = 0;
	};
}