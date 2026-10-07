#pragma once

#include "IMultiplexer.h"

namespace Proxirae {
	class DispatcherMultiplexer : public IMultiplexer {
	public:
		void Register(IMultiplexer& multiplexer);

		bool Start() override;
		bool Stop() override;

		std::optional<std::vector<FlowContract>> GetActiveFlows() override;
		void TerminateFlow(const std::string& id) override;

	private:
		std::vector<IMultiplexer*> m_multiplexers;
	};
}