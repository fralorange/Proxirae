#pragma once

#include <optional>
#include <vector>
#include <string>

#include "contracts/flow/FlowContract.h"

namespace Proxirae {
	class IMultiplexer {
	public:
		virtual ~IMultiplexer() = default;

		virtual bool Start() = 0;
		virtual bool Stop() = 0;

		virtual std::optional<std::vector<FlowContract>> GetActiveFlows() = 0;
		virtual void TerminateFlow(const std::string& id) = 0;
	};
}