#include "features/interception/correlation/win/WinDnsCorrelator.h"
#include "features/interception/diversion/win/WinPacketContext.h"
#include "platform/environment/inet.h"

namespace Proxirae {
	struct DnsHeader {
		std::uint16_t id;
		std::uint16_t flags;
		std::uint16_t qdCount;
		std::uint16_t anCount;
		std::uint16_t nsCount;
		std::uint16_t arCount;
	}; // Move DnsHeader in DnsCorrelator base class when implementing cross-platform support for DNS correlation.

	WinDnsCorrelator::WinDnsCorrelator(DomainTable& domains, std::uint16_t redirectPort)
		: m_domains(domains), m_redirectPort(redirectPort) { }

	bool WinDnsCorrelator::CorrelateNetwork(const Packet& packet, const std::function<void(IPacketContext&)>& callback)
	{
		auto ctxOpt = WinPacketContext::TryCreate(packet.data.data(), packet.length, packet.metadata);

		if (!ctxOpt.has_value() || !ctxOpt->IsUdp()) {
			return false;
		}

		auto& context = ctxOpt.value();

		const bool isDnsResponse = context.GetSourcePort() == 53;

		if (!isDnsResponse) {
			return false;
		}

		auto dnsData = context.GetPayloadData();
		auto dnsDataLen = context.GetPayloadLength();

		ParseAndRegisterDns(dnsData, dnsDataLen);

		return false; // Dns Correlator is responsible for sniffing only, so we pass packet to the next correlator in the chain.
	}

	bool WinDnsCorrelator::CorrelateSocket(const PacketMetadata& /*metadata*/, const std::function<void(IPacketContext&)>& /*callback*/)
	{
		return false; // Same reason as above, we don't handle socket events for DNS in this correlator.
	}

	std::optional<std::string> WinDnsCorrelator::ReadDomainName(const std::uint8_t* buffer, std::size_t length, std::size_t& offset)
	{
		std::string domain;
		std::size_t currentOffset = offset;
		bool jumped = false;
		std::size_t originalOffsetAfterFirstJump = 0;
		int jumpCount = 0;
		constexpr int maxJumps = 5;

		while (currentOffset < length) {
			std::uint8_t len = buffer[currentOffset];

			if (len == 0) {
				if (!jumped) {
					offset = currentOffset + 1;
				}
				break;
			}

			if ((len & 0xC0) == 0xC0) {
				if (currentOffset + 1 >= length) {
					return std::nullopt;
				}
				
				std::uint16_t pointer = ((len & 0x3F) << 8) | buffer[currentOffset + 1];
				if (pointer >= length) {
					return std::nullopt;
				}
				
				if (!jumped) {
					originalOffsetAfterFirstJump = currentOffset + 2;
					jumped = true;
				}

				if (++jumpCount > maxJumps) {
					return std::nullopt;
				}

				currentOffset = pointer;
			} else {
				currentOffset++;
				if (currentOffset + len > length) {
					return std::nullopt;
				}

				if (!domain.empty()) {
					domain.append(".");
				}
				
				domain.append(reinterpret_cast<const char*>(buffer + currentOffset), len);
				currentOffset += len;
			}
		}

		if (jumped) {
			offset = originalOffsetAfterFirstJump;
		}

		return domain.empty() ? std::nullopt : std::make_optional(domain);
	}

	void WinDnsCorrelator::ParseAndRegisterDns(const std::uint8_t* payload, std::size_t length)
	{
		if (payload == nullptr || length < sizeof(DnsHeader)) {
			return;
		}

		auto* header = reinterpret_cast<const DnsHeader*>(payload);

		std::uint16_t flags = ntohs(header->flags);
		std::uint16_t qdCount = ntohs(header->qdCount);
		std::uint16_t anCount = ntohs(header->anCount);

		bool isResponse = (flags & 0x8000) != 0;
		std::uint8_t rcode = flags & 0x000F;

		if (!isResponse || rcode != 0 || anCount == 0) {
			return;
		}

		std::size_t offset = sizeof(DnsHeader);

		for (std::uint16_t i = 0; i < qdCount; ++i) {
			auto qName = ReadDomainName(payload, length, offset);

			if (!qName.has_value()) {
				return;
			}

			if (offset + 4 > length) {
				return;
			}

			offset += 4;
		}

		for (std::uint16_t i = 0; i < anCount; ++i) {
			auto domain = ReadDomainName(payload, length, offset);
			
			if (!domain.has_value()) {
				return;
			}

			std::uint16_t type = ntohs(*reinterpret_cast<const std::uint16_t*>(payload + offset));
			std::uint32_t ttl = ntohl(*reinterpret_cast<const std::uint32_t*>(payload + offset + 4));
			std::uint16_t dataLen = ntohs(*reinterpret_cast<const std::uint16_t*>(payload + offset + 8));

			offset += 10;

			if (offset + dataLen > length) {
				return;
			}

			if (type == 1 && dataLen == 4) {
				IpAddress ip;
				ip.isIPv6 = false;
				std::memcpy(&ip.data[0], payload + offset, 4);
				ip.data[0] = ntohl(ip.data[0]);

				m_domains.Add(ip, *domain, ttl);
			}

			else if (type == 28 && dataLen == 16) {
				IpAddress ip;
				ip.isIPv6 = true;
				std::memcpy(ip.data.data(), payload + offset, 16);

				for (auto& word : ip.data) {
					word = ntohl(word);
				}

				m_domains.Add(ip, *domain, ttl);
			}

			offset += dataLen;
		}
	}
}