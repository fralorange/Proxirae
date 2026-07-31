#pragma once

#include <windivert.h>
#include <optional>

#include "core/packet/IPacketContext.h"

namespace Proxirae {
	class WinPacketContext : public IPacketContext {
	public:
		static std::optional<WinPacketContext> TryCreate(const std::uint8_t* buffer, std::uint32_t len, WINDIVERT_ADDRESS addr);

		bool IsTcp() const override;
		bool IsTcpSyn() const override;
		bool IsTcpAck() const override;
		bool IsTcpRst() const override;
		bool IsTcpFin() const override;

		bool IsUdp() const override;

		bool IsOutbound() const override;
		bool IsLoopback() const override;
		bool IsModified() const override;

		std::uint32_t GetSourceAddress() const override;
		std::uint32_t GetDestinationAddress() const override;

		std::uint16_t GetSourcePort() const override;
		std::uint16_t GetDestinationPort() const override;

		std::uint8_t GetProtocol() const override;

		Endpoint GetSourceEndpoint() const override;
		Endpoint GetDestinationEndpoint() const override;

		void SetSource(std::uint32_t addr, std::uint16_t port) override;
		void SetDestination(std::uint32_t addr, std::uint16_t port) override;
	private:
		WINDIVERT_ADDRESS m_addr;
		PWINDIVERT_IPHDR m_ipHdr;
		std::uint8_t m_protocol;
		PWINDIVERT_TCPHDR m_tcpHdr;
		PWINDIVERT_UDPHDR m_udpHdr;

		bool m_isModified{ false };

		WinPacketContext(WINDIVERT_ADDRESS addr, PWINDIVERT_IPHDR ipHdr, std::uint8_t protocol, PWINDIVERT_TCPHDR tcpHdr, PWINDIVERT_UDPHDR udpHdr);
	};
}