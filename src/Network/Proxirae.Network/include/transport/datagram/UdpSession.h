#pragma once

#include <memory>
#include <chrono>
#include <string>
#include <string_view>

#include "environment/sock_types.h"
#include "primitives/endpoints/Endpoint.h"
#include "proxification/IProxyFactory.h"
#include "diagnostics/ILogger.h"
#include "primitives/tuples/FiveTuple.h"
#include "persistence/connections/ConnectionEntry.h"
#include "contracts/flow/FlowContract.h"
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
		std::uint32_t GetAddress() const;
		std::uint16_t GetPort() const;

	private:
		class UdpBridge;
		std::unique_ptr<UdpBridge> m_bridge;

		std::chrono::steady_clock::time_point m_start;

		std::string m_id;
	};
}