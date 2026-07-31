#pragma once

#include "core/packet/IPacketHandler.h"
#include "core/registry/ConnectionTable.h"
#include "diagnostics/ILogger.h"

namespace Proxirae {
	class TcpHandler : public IPacketHandler {
	public:
		TcpHandler(std::uint16_t redirectPort, ConnectionTable& connections, ILogger& logger);

		bool CanHandle(const IPacketContext& ctx) override;
		void Handle(IPacketContext& ctx) override;

	private:
		ConnectionTable& m_connections;
		ILogger& m_logger;
		std::uint16_t m_redirectPort;

		void HandleSynOnly(IPacketContext& ctx);
		void HandleSynAckOnly(IPacketContext& ctx);
		void HandleAckOnly(IPacketContext& ctx);

		void HandleRst(IPacketContext& ctx);
		void HandleFin(IPacketContext& ctx);
	};
}