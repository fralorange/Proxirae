#pragma once

#include <memory>
#include <chrono>
#include <string>
#include <string_view>

#include "platform/environment/sock_types.h"
#include "core/primitives/endpoints/Endpoint.h"
#include "features/proxification/IProxyFactory.h"
#include "features/diagnostics/ILogger.h"
#include "core/primitives/ip/IpAddress.h"
#include "core/primitives/tuples/FiveTuple.h"
#include "features/persistence/connections/ConnectionEntry.h"
#include "core/contracts/flow/FlowContract.h"
#include "asyncio/io/datagram/IIoDatagramAdapter.h"

namespace Proxirae {
	class UdpSession : public std::enable_shared_from_this<UdpSession> {
	public:
		UdpSession(NativeSocket shared, Endpoint endpoint, IIoDatagramAdapter& adapter, IProxyFactory& factory, ILogger& logger);
		~UdpSession();

		bool Establish(const FiveTuple& key, const ConnectionEntry& entry);
		void Terminate();

		bool IsExpired(std::chrono::seconds timeout) const;

		void OnData(std::span<const std::byte> payload);

		FlowContract GetFlow() const;

		std::string_view GetId() const;
		IpAddress GetAddress() const;
		std::uint16_t GetPort() const;

	private:
		class UdpBridge;
		std::unique_ptr<UdpBridge> m_bridge;

		std::chrono::steady_clock::time_point m_start;

		std::string m_id;
	};
}