#include <WS2tcpip.h>

#include "interception/diversion/win/WinPacketContext.h"

namespace Proxirae {
	WinPacketContext::WinPacketContext(std::uint8_t* rawData, std::uint32_t rawDataLen, PacketMetadata metadata, PWINDIVERT_IPHDR ipHdr, PWINDIVERT_IPV6HDR ipv6Hdr, UINT8 protocol, PWINDIVERT_TCPHDR tcpHdr, PWINDIVERT_UDPHDR udpHdr, std::uint32_t payloadLen)
		: m_rawData(rawData), 
		  m_rawDataLen(rawDataLen), 
		  m_metadata(metadata), 
		  m_ipHdr(ipHdr),
		  m_ipv6Hdr(ipv6Hdr),
		  m_protocol(protocol), 
		  m_tcpHdr(tcpHdr), 
		  m_udpHdr(udpHdr), 
		  m_payloadLen(payloadLen)
	{}

	std::optional<WinPacketContext> WinPacketContext::TryCreate(const std::uint8_t* buffer, std::uint32_t len, PacketMetadata metadata)
	{
		PWINDIVERT_IPHDR ipHdr;
		PWINDIVERT_IPV6HDR ipv6Hdr;
		std::uint8_t protocol;
		PWINDIVERT_TCPHDR tcpHdr;
		PWINDIVERT_UDPHDR udpHdr;
		std::uint32_t payloadLen;

		if (!WinDivertHelperParsePacket(
			buffer,
			len,
			&ipHdr,
			&ipv6Hdr,
			&protocol,
			nullptr, nullptr,
			&tcpHdr,
			&udpHdr,
			nullptr,
			&payloadLen,
			nullptr,
			nullptr
		)) {
			return std::nullopt;
		}

		if ((!ipHdr && !ipv6Hdr) || (!tcpHdr && !udpHdr)) {
			return std::nullopt;
		}

		return WinPacketContext(const_cast<std::uint8_t*>(buffer), len, metadata, ipHdr, ipv6Hdr, protocol, tcpHdr, udpHdr, payloadLen);
	}

	bool WinPacketContext::IsIPv6() const
	{
		return m_ipv6Hdr != nullptr;
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
		return m_metadata.Outbound;
	}

	bool WinPacketContext::IsLoopback() const
	{
		return m_metadata.Loopback;
	}

	bool WinPacketContext::IsModified() const
	{
		return m_isModified;
	}

	bool WinPacketContext::HasPayload() const
	{
		return m_payloadLen > 0;
	}

	IpAddress WinPacketContext::GetSourceAddress() const
	{
		IpAddress addr{};

		if (m_ipv6Hdr) {
			addr.isIPv6 = true;
			WinDivertHelperNtohIpv6Address(m_ipv6Hdr->SrcAddr, addr.data.data());
		}
		else if (m_ipHdr) {
			addr.isIPv6 = false;
			addr.data[0] = WinDivertHelperNtohl(m_ipHdr->SrcAddr);
		}

		return addr;
	}

	IpAddress WinPacketContext::GetDestinationAddress() const
	{
		IpAddress addr{};

		if (m_ipv6Hdr) {
			addr.isIPv6 = true;
			WinDivertHelperNtohIpv6Address(m_ipv6Hdr->DstAddr, addr.data.data());
		}
		else if (m_ipHdr) {
			addr.isIPv6 = false;
			addr.data[0] = WinDivertHelperNtohl(m_ipHdr->DstAddr);
		}

		return addr;
	}

	std::uint16_t WinPacketContext::GetSourcePort() const
	{
		if (m_protocol == IPPROTO_TCP) {
			return WinDivertHelperNtohs(m_tcpHdr->SrcPort);
		}
		else if (m_protocol == IPPROTO_UDP) {
			return WinDivertHelperNtohs(m_udpHdr->SrcPort);
		}

		return 0;
	}

	std::uint16_t WinPacketContext::GetDestinationPort() const
	{
		if (m_protocol == IPPROTO_TCP) {
			return WinDivertHelperNtohs(m_tcpHdr->DstPort);
		}
		else if (m_protocol == IPPROTO_UDP) {
			return WinDivertHelperNtohs(m_udpHdr->DstPort);
		}

		return 0;
	}

	std::uint8_t WinPacketContext::GetProtocol() const
	{
		return m_protocol;
	}

	Endpoint WinPacketContext::GetSourceEndpoint() const
	{
		return Endpoint(GetSourceAddress(), GetSourcePort());
	}

	Endpoint WinPacketContext::GetDestinationEndpoint() const
	{
		return Endpoint(GetDestinationAddress(), GetDestinationPort());
	}

	std::optional<std::uint32_t> WinPacketContext::GetProcessId() const
	{
		return m_processId;
	}

	std::uint8_t* WinPacketContext::GetRawData()
	{
		return m_rawData;
	}

	std::uint32_t WinPacketContext::GetRawDataLength() const
	{
		return m_rawDataLen;
	}

	PacketMetadata& WinPacketContext::GetMetadata()
	{
		return m_metadata;
	}

	void WinPacketContext::SetSource(const IpAddress& addr, std::uint16_t port)
	{
		if (m_ipv6Hdr && addr.isIPv6) {
			WinDivertHelperHtonIpv6Address(addr.data.data(), m_ipv6Hdr->SrcAddr);
		}
		else if (m_ipHdr && !addr.isIPv6) {
			m_ipHdr->SrcAddr = WinDivertHelperHtonl(addr.data[0]);
		}

		if (m_protocol == IPPROTO_TCP) {
			m_tcpHdr->SrcPort = WinDivertHelperHtons(port);
		}
		else if (m_protocol == IPPROTO_UDP) {
			m_udpHdr->SrcPort = WinDivertHelperHtons(port);
		}

		m_isModified = true;
	}

	void WinPacketContext::SetDestination(const IpAddress& addr, std::uint16_t port)
	{
		if (m_ipv6Hdr && addr.isIPv6) {
			WinDivertHelperHtonIpv6Address(addr.data.data(), m_ipv6Hdr->DstAddr);
		}
		else if (m_ipHdr && !addr.isIPv6) {
			m_ipHdr->DstAddr = WinDivertHelperHtonl(addr.data[0]);
		}

		if (m_protocol == IPPROTO_TCP) {
			m_tcpHdr->DstPort = WinDivertHelperHtons(port);
		}
		else if (m_protocol == IPPROTO_UDP) {
			m_udpHdr->DstPort = WinDivertHelperHtons(port);
		}

		m_isModified = true;
	}

	void WinPacketContext::SetProcessId(std::uint32_t pid)
	{
		if (m_processId.has_value()) {
			return;
		}

		m_processId = pid;
	}
}