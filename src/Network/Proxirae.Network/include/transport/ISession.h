#pragma once

#include <memory>
#include <functional>
#include <cstdint>
#include <string_view>

#include "persistence/FiveTuple.h"
#include "persistence/ConnectionEntry.h"
#include "contracts/flow/FlowContract.h"

namespace Proxirae {
	class ISession {
	public:
		using TerminationCallback = std::function<void(std::shared_ptr<ISession>)>;

		virtual ~ISession() = default;

		virtual void Establish(const FiveTuple& key, const ConnectionEntry& entry, TerminationCallback onTerminated) = 0;
		virtual void Terminate() = 0;

		virtual FlowContract GetFlow() const = 0;

		virtual std::string_view GetId() const = 0;
		virtual std::uint32_t GetAddress() const = 0;
		virtual std::uint16_t GetPort() const = 0;
	};
}