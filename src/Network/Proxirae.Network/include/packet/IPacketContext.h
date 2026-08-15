#pragma once

#include <optional>

#include "packet/Endpoint.h"
#include "packet/PacketMetadata.h"

namespace Proxirae {
	class IPacketContext {
	public:
		virtual ~IPacketContext() = default;

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

		virtual std::uint32_t GetSourceAddress() const = 0;
		virtual std::uint32_t GetDestinationAddress() const = 0;

		virtual std::uint16_t GetSourcePort() const = 0;
		virtual std::uint16_t GetDestinationPort() const = 0;

		virtual std::uint8_t GetProtocol() const = 0;

		virtual Endpoint GetSourceEndpoint() const = 0;
		virtual Endpoint GetDestinationEndpoint() const = 0;

		virtual std::optional<std::uint32_t> GetProcessId() const = 0;

		virtual std::uint8_t* GetRawData() = 0;
		virtual std::uint32_t GetRawDataLength() const = 0;
		virtual PacketMetadata& GetMetadata() = 0;

		virtual void SetSource(std::uint32_t addr, std::uint16_t port) = 0;
		virtual void SetDestination(std::uint32_t addr, std::uint16_t port) = 0;

		virtual void SetProcessId(std::uint32_t pid) = 0;
	};
}