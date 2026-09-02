#pragma once

#include <memory>
#include <chrono>
#include <string_view>

#include "ISession.h"
#include "environment/sock_types.h"
#include "asyncio/IIoDriver.h"
#include "diagnostics/ILogger.h"
#include "persistence/ConnectionEntry.h"
#include "interception/diversion/Endpoint.h"
#include "proxification/IProxyFactory.h"
#include "persistence/FiveTuple.h"
#include "contracts/flow/FlowContract.h"

namespace Proxirae {
	class TcpSession : public ISession, public std::enable_shared_from_this<TcpSession> {
	public:
		TcpSession(NativeSocket client, Endpoint endpoint, IIoDriver& driver, ILogger& logger, IProxyFactory& factory);
		~TcpSession() override;

		void Establish(const FiveTuple& key, const ConnectionEntry& entry, TerminationCallback onTerminated) override;
		void Terminate() override;

		FlowContract GetFlow() const override;

		std::string_view GetId() const override;
		std::uint32_t GetAddress() const override;
		std::uint16_t GetPort() const override;

	private:
		class TcpBridge;
		std::unique_ptr<TcpBridge> m_bridge;

		std::chrono::steady_clock::time_point m_start;

		std::string m_id;
	};
}