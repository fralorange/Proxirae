#include "interception/correlation/DispatcherCorrelator.h"

namespace Proxirae {
	void DispatcherCorrelator::Register(IPacketCorrelator& correlator)
	{
		m_correlators.push_back(&correlator);
	}

	bool DispatcherCorrelator::CorrelateNetwork(const Packet& packet, const std::function<void(IPacketContext&)>& callback)
	{
		for (auto* correlator : m_correlators) {
			if (correlator->CorrelateNetwork(packet, callback)) {
				return true;
			}
		}

		return false;
	}

	bool DispatcherCorrelator::CorrelateSocket(const PacketMetadata& metadata, const std::function<void(IPacketContext&)>& callback)
	{
		for (auto* correlator : m_correlators) {
			if (correlator->CorrelateSocket(metadata, callback)) {
				return true;
			}
		}

		return false;
	}
}