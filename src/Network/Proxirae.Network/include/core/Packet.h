#pragma once

#include <windivert.h>
#include <optional>

#include "core/Endpoint.h"

namespace Proxirae {
	class Packet {
	public:
		static std::optional<Packet> TryCreate(const UINT8* buffer, UINT32 len, WINDIVERT_ADDRESS addr);

		bool IsTcp() const;
		bool IsTcpSyn() const;
		bool IsTcpAck() const;
		bool IsTcpRst() const;
		bool IsTcpFin() const;

		bool IsUdp() const;

		bool IsOutbound() const;
		bool IsLoopback() const;
		bool IsModified() const;

		UINT32 GetSourceAddress() const;
		UINT32 GetDestinationAddress() const;

		UINT16 GetSourcePort() const;
		UINT16 GetDestinationPort() const;

		UINT8 GetProtocol() const;

		Endpoint GetSourceEndpoint() const;
		Endpoint GetDestinationEndpoint() const;

		void SetSource(UINT32 addr, UINT16 port);
		void SetDestination(UINT32 addr, UINT16 port);
	private:
		WINDIVERT_ADDRESS m_addr;
		PWINDIVERT_IPHDR m_ipHdr;
		UINT8 m_protocol;
		PWINDIVERT_TCPHDR m_tcpHdr;
		PWINDIVERT_UDPHDR m_udpHdr;

		bool m_isModified{ false };

		Packet(WINDIVERT_ADDRESS addr, PWINDIVERT_IPHDR ipHdr, UINT8 protocol, PWINDIVERT_TCPHDR tcpHdr, PWINDIVERT_UDPHDR udpHdr);
	};
}