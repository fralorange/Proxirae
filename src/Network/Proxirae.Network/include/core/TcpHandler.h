#pragma once

#include "core/IPacketHandler.h"
#include "registry/ConnectionTable.h"
#include "diagnostics/ILogger.h"

namespace Proxirae {
	class TcpHandler : public IPacketHandler {
	public:
		TcpHandler(ConnectionTable& connections, ILogger& logger);

		bool CanHandle(const Packet& packet) override;
		void Handle(Packet& packet) override;

	private:
		ConnectionTable& m_connections;
		ILogger& m_logger;

		void HandleSynOnly(Packet& packet);
		void HandleSynAckOnly(Packet& packet);
		void HandleAckOnly(Packet& packet);

		void HandleRst(Packet& packet);
		void HandleFin(Packet& packet);
	};
}