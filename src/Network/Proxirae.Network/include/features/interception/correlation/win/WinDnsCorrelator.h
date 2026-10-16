#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include "features/persistence/domains/DomainTable.h"
#include "features/interception/correlation/IPacketCorrelator.h"

namespace Proxirae {
	class WinDnsCorrelator : public IPacketCorrelator {
	public:
		WinDnsCorrelator(DomainTable& domains, std::uint16_t redirectPort);

		bool CorrelateNetwork(const Packet& packet, const std::function<void(IPacketContext&)>& callback) override;
		bool CorrelateSocket(const PacketMetadata& metadata, const std::function<void(IPacketContext&)>& callback) override;

	private:
		std::optional<std::string> ReadDomainName(const std::uint8_t* buffer, std::size_t length, std::size_t& offset);
		void ParseAndRegisterDns(const std::uint8_t* payload, std::size_t length);

	private:
		DomainTable& m_domains;
		std::uint16_t m_redirectPort;
	};
}