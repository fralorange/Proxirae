#pragma once

#include <vector>

#include "interception/correlation/IPacketCorrelator.h"

namespace Proxirae {
	class DispatcherCorrelator : public IPacketCorrelator {
	public:
		void Register(IPacketCorrelator& correlator);

		bool CorrelateNetwork(const Packet& packet, const std::function<void(IPacketContext&)>& callback) override;
		bool CorrelateSocket(const PacketMetadata& metadata, const std::function<void(IPacketContext&)>& callback) override;

	private:
		std::vector<IPacketCorrelator*> m_correlators{};
	};
}