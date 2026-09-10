#include "transport/DispatcherMultiplexer.h"

namespace Proxirae {
	void DispatcherMultiplexer::Register(IMultiplexer& multiplexer)
	{
		m_multiplexers.push_back(&multiplexer);
	}

	bool DispatcherMultiplexer::Start()
	{
		for (auto* m : m_multiplexers) {
			if (!m->Start()) {
				Stop();

				return false;
			}
		}

		return true;
	}

	bool DispatcherMultiplexer::Stop()
	{
		bool success = true;

		for (auto* m : m_multiplexers) {
			if (!m->Stop()) {
				success = false;
			}
		}

		return success;
	}

	std::optional<std::vector<FlowContract>> DispatcherMultiplexer::GetActiveFlows()
	{
		std::vector<FlowContract> combined;

		for (auto* m : m_multiplexers) {
			auto flows = m->GetActiveFlows();

			if (flows.has_value()) {
				combined.insert(combined.end(), std::make_move_iterator(flows->begin()), std::make_move_iterator(flows->end()));
			}
		}

		return combined.empty() ? std::nullopt : std::make_optional(std::move(combined));
	}

	void DispatcherMultiplexer::TerminateFlow(const std::string& id)
	{
		for (auto* m : m_multiplexers) {
			m->TerminateFlow(id);
		}
	}
}