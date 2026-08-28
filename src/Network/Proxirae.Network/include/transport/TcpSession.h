#pragma once

#include <memory>
#include <chrono>
#include <string_view>

#include "environment/sock_types.h"
#include "asyncio/IIoDriver.h"
#include "diagnostics/ILogger.h"
#include "persistence/ConnectionEntry.h"
#include "interception/Endpoint.h"
#include "proxification/IProxyFactory.h"
#include "persistence/FiveTuple.h"
#include "contracts/flow/FlowContract.h"

namespace Proxirae {
	class TcpSession : public std::enable_shared_from_this<TcpSession> {
	public:
		TcpSession(NativeSocket client, Endpoint endpoint, IIoDriver& driver, ILogger& logger, IProxyFactory& factory);
		~TcpSession();

		std::string_view GetId() const;
		std::uint32_t GetAddress() const;
		std::uint16_t GetPort() const;

		FlowContract GetFlow() const;

		void Establish(const FiveTuple& key, const ConnectionEntry& entry, std::function<void(std::shared_ptr<TcpSession>)> onTerminated);
		void Terminate();

	private:
		class TcpBridge;
		std::unique_ptr<TcpBridge> m_bridge;

		std::chrono::steady_clock::time_point m_start;

		std::string m_id;
	};
}