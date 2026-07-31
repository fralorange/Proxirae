#include <WS2tcpip.h>

#include "core/packet/win/WinPacketContext.h"

namespace Proxirae {
	WinPacketContext::WinPacketContext(WINDIVERT_ADDRESS addr, PWINDIVERT_IPHDR ipHdr, UINT8 protocol, PWINDIVERT_TCPHDR tcpHdr, PWINDIVERT_UDPHDR udpHdr)
		: m_addr(addr), m_ipHdr(ipHdr), m_protocol(protocol), m_tcpHdr(tcpHdr), m_udpHdr(udpHdr)
	{}

	std::optional<WinPacketContext> WinPacketContext::TryCreate(const std::uint8_t* buffer, std::uint32_t len, WINDIVERT_ADDRESS addr)
	{
		PWINDIVERT_IPHDR ipHdr;
		std::uint8_t protocol;
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

		return WinPacketContext(addr, ipHdr, protocol, tcpHdr, udpHdr);
	}

	bool WinPacketContext::IsTcp() const
	{
		return m_protocol == IPPROTO_TCP;
	}

	bool WinPacketContext::IsTcpSyn() const {
		return m_tcpHdr && m_tcpHdr->Syn;
	}

	bool WinPacketContext::IsTcpAck() const
	{
		return m_tcpHdr && m_tcpHdr->Ack;
	}

	bool WinPacketContext::IsTcpRst() const
	{
		return m_tcpHdr && m_tcpHdr->Rst;
	}

	bool WinPacketContext::IsTcpFin() const
	{
		return m_tcpHdr && m_tcpHdr->Fin;
	}

	bool WinPacketContext::IsUdp() const {
		return m_protocol == IPPROTO_UDP;
	}

	bool WinPacketContext::IsOutbound() const
	{
		return m_addr.Outbound;
	}

	bool WinPacketContext::IsLoopback() const
	{
		return m_addr.Loopback;
	}

	bool WinPacketContext::IsModified() const
	{
		return m_isModified;
	}

	std::uint32_t WinPacketContext::GetSourceAddress() const
	{
		return m_ipHdr->SrcAddr;
	}

	std::uint32_t WinPacketContext::GetDestinationAddress() const
	{
		return m_ipHdr->DstAddr;
	}

	std::uint16_t WinPacketContext::GetSourcePort() const
	{
		if (m_protocol == IPPROTO_TCP) {
			return m_tcpHdr->SrcPort;
		}
		else if (m_protocol == IPPROTO_UDP) {
			return m_udpHdr->SrcPort;
		}

		return 0;
	}

	std::uint16_t WinPacketContext::GetDestinationPort() const
	{
		if (m_protocol == IPPROTO_TCP) {
			return m_tcpHdr->DstPort;
		}
		else if (m_protocol == IPPROTO_UDP) {
			return m_udpHdr->DstPort;
		}

		return 0;
	}

	std::uint8_t WinPacketContext::GetProtocol() const
	{
		return m_protocol;
	}

	Endpoint WinPacketContext::GetSourceEndpoint() const
	{
		return Endpoint(m_ipHdr->SrcAddr, m_tcpHdr ? m_tcpHdr->SrcPort : m_udpHdr->SrcPort);
	}

	Endpoint WinPacketContext::GetDestinationEndpoint() const
	{
		return Endpoint(m_ipHdr->DstAddr, m_tcpHdr ? m_tcpHdr->DstPort : m_udpHdr->DstPort);
	}

	void WinPacketContext::SetSource(std::uint32_t addr, std::uint16_t port)
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

	void WinPacketContext::SetDestination(std::uint32_t addr, std::uint16_t port)
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