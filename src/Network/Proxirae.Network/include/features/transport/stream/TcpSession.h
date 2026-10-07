#pragma once

#include <memory>
#include <chrono>
#include <string_view>

#include "asyncio/io/stream/IIoStreamAdapter.h"
#include "platform/environment/sock_types.h"
#include "features/diagnostics/ILogger.h"
#include "features/persistence/connections/ConnectionEntry.h"
#include "core/primitives/endpoints/Endpoint.h"
#include "features/proxification/IProxyFactory.h"
#include "core/primitives/tuples/FiveTuple.h"
#include "core/primitives/ip/IpAddress.h"
#include "core/contracts/flow/FlowContract.h"

namespace Proxirae {
	class TcpSession : public std::enable_shared_from_this<TcpSession> {
	public:
		using TerminationCallback = std::function<void(std::shared_ptr<TcpSession>)>;

		TcpSession(NativeSocket client, Endpoint endpoint, IIoStreamAdapter& adapter, ILogger& logger, IProxyFactory& factory);
		~TcpSession();

		bool Establish(const FiveTuple& key, const ConnectionEntry& entry, TerminationCallback onTerminated);
		void Terminate();

		FlowContract GetFlow() const;

		std::string_view GetId() const;
		IpAddress GetAddress() const;
		std::uint16_t GetPort() const;

	private:
		class TcpBridge;
		std::unique_ptr<TcpBridge> m_bridge;

		std::chrono::steady_clock::time_point m_start;

		std::string m_id;
	};
}