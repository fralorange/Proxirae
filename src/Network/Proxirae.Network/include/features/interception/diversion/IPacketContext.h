#pragma once

#include <optional>

#include "core/primitives/endpoints/Endpoint.h"
#include "core/primitives/ip/IpAddress.h"
#include "features/interception/diversion/PacketMetadata.h"

namespace Proxirae {
	class IPacketContext {
	public:
		virtual ~IPacketContext() = default;

		virtual bool IsIPv6() const = 0;

		virtual bool IsTcp() const = 0;
		virtual bool IsTcpSyn() const = 0;
		virtual bool IsTcpAck() const = 0;
		virtual bool IsTcpRst() const = 0;
		virtual bool IsTcpFin() const = 0;

		virtual bool IsUdp() const = 0;

		virtual bool IsOutbound() const = 0;
		virtual bool IsLoopback() const = 0;
		virtual bool IsModified() const = 0;

		virtual bool HasPayload() const = 0;

		// Must be in Host Byte Order

		virtual IpAddress GetSourceAddress() const = 0;
		virtual IpAddress GetDestinationAddress() const = 0;

		// Must be in Host Byte Order

		virtual std::uint16_t GetSourcePort() const = 0;
		virtual std::uint16_t GetDestinationPort() const = 0;

		virtual std::uint8_t GetProtocol() const = 0;

		// Must be in Host Byte Order

		virtual Endpoint GetSourceEndpoint() const = 0;
		virtual Endpoint GetDestinationEndpoint() const = 0;

		virtual std::optional<std::uint32_t> GetProcessId() const = 0;

		virtual std::uint8_t* GetRawData() = 0;
		virtual std::uint32_t GetRawDataLength() const = 0;
		virtual PacketMetadata& GetMetadata() = 0;

		// Must operate in Host Byte Order

		virtual void SetSource(const IpAddress& addr, std::uint16_t port) = 0;
		virtual void SetDestination(const IpAddress& addr, std::uint16_t port) = 0;

		virtual void SetProcessId(std::uint32_t pid) = 0;
	};
}