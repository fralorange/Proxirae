#include <WS2tcpip.h>
#include <stdexcept>

#include "core/Packet.h"

namespace Proxirae {
	Packet::Packet(WINDIVERT_ADDRESS addr, PWINDIVERT_IPHDR ipHdr, UINT8 protocol, PWINDIVERT_TCPHDR tcpHdr, PWINDIVERT_UDPHDR udpHdr)
		: m_addr(addr), m_ipHdr(ipHdr), m_protocol(protocol), m_tcpHdr(tcpHdr), m_udpHdr(udpHdr)
	{}

	std::optional<Packet> Packet::TryCreate(const UINT8* buffer, const UINT32 len, WINDIVERT_ADDRESS addr)
	{
		PWINDIVERT_IPHDR ipHdr;
		UINT8 protocol;
		PWINDIVERT_TCPHDR tcpHdr;
		PWINDIVERT_UDPHDR udpHdr;

		if (!WinDivertHelperParsePacket(
			buffer,
			len,
			&ipHdr,
			nullptr,
			&protocol,
			nullptr, nullptr,
			&tcpHdr,
			&udpHdr,
			nullptr,
			nullptr,
			nullptr,
			nullptr
		)) {
			return std::nullopt;
		}

		if (!ipHdr || (!tcpHdr && !udpHdr)) {
			return std::nullopt;
		}

		return Packet(addr, ipHdr, protocol, tcpHdr, udpHdr);
	}

	bool Packet::IsTcp() const
	{
		return m_protocol == IPPROTO_TCP;
	}

	bool Packet::IsTcpSyn() const {
		return m_tcpHdr && m_tcpHdr->Syn;
	}

	bool Packet::IsTcpAck() const
	{
		return m_tcpHdr && m_tcpHdr->Ack;
	}

	bool Packet::IsTcpRst() const
	{
		return m_tcpHdr && m_tcpHdr->Rst;
	}

	bool Packet::IsTcpFin() const
	{
		return m_tcpHdr && m_tcpHdr->Fin;
	}

	bool Packet::IsUdp() const {
		return m_protocol == IPPROTO_UDP;
	}

	bool Packet::IsOutbound() const
	{
		return m_addr.Outbound;
	}

	bool Packet::IsLoopback() const
	{
		return m_addr.Loopback;
	}

	bool Packet::IsModified() const
	{
		return m_isModified;
	}

	UINT32 Packet::GetSourceAddress() const
	{
		return m_ipHdr->SrcAddr;
	}

	UINT32 Packet::GetDestinationAddress() const
	{
		return m_ipHdr->DstAddr;
	}

	UINT16 Packet::GetSourcePort() const
	{
		if (m_protocol == IPPROTO_TCP) {
			return m_tcpHdr->SrcPort;
		}
		else if (m_protocol == IPPROTO_UDP) {
			return m_udpHdr->SrcPort;
		}

		return 0;
	}

	UINT16 Packet::GetDestinationPort() const
	{
		if (m_protocol == IPPROTO_TCP) {
			return m_tcpHdr->DstPort;
		}
		else if (m_protocol == IPPROTO_UDP) {
			return m_udpHdr->DstPort;
		}

		return 0;
	}

	UINT8 Packet::GetProtocol() const
	{
		return m_protocol;
	}

	Endpoint Packet::GetSourceEndpoint() const
	{
		return Endpoint(m_ipHdr->SrcAddr, m_tcpHdr ? m_tcpHdr->SrcPort : m_udpHdr->SrcPort);
	}

	Endpoint Packet::GetDestinationEndpoint() const
	{
		return Endpoint(m_ipHdr->DstAddr, m_tcpHdr ? m_tcpHdr->DstPort : m_udpHdr->DstPort);
	}

	void Packet::SetSource(UINT32 addr, UINT16 port)
	{
		m_ipHdr->SrcAddr = addr;

		if (m_protocol == IPPROTO_TCP) {
			m_tcpHdr->SrcPort = port;
		}
		else if (m_protocol == IPPROTO_UDP) {
			m_udpHdr->SrcPort = port;
		}

		m_isModified = true;
	}

	void Packet::SetDestination(UINT32 addr, UINT16 port)
	{
		m_ipHdr->DstAddr = addr;

		if (m_protocol == IPPROTO_TCP) {
			m_tcpHdr->DstPort = port;
		}
		else if (m_protocol == IPPROTO_UDP) {
			m_udpHdr->DstPort = port;
		}

		m_isModified = true;
	}
}