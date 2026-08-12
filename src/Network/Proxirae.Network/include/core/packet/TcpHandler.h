#pragma once

#include "core/packet/IPacketHandler.h"
#include "core/registry/ConnectionRegistry.h"
#include "diagnostics/ILogger.h"

namespace Proxirae {
	class TcpHandler : public IPacketHandler {
	public:
		TcpHandler(std::uint16_t redirectPort, ConnectionRegistry& connections, ILogger& logger);

		bool CanHandle(const IPacketContext& ctx) override;
		void Handle(PacketDispatchContext& ctx) override;

	private:
		ConnectionRegistry& m_connections;
		ILogger& m_logger;
		std::uint16_t m_redirectPort;

		void HandleSynOnly(PacketDispatchContext& ctx);
		void HandleSynAckOnly(IPacketContext& ctx);
		void HandleAckOnly(IPacketContext& ctx);

		void HandleRst(IPacketContext& ctx);
		void HandleFin(IPacketContext& ctx);
	};
}