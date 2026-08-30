#pragma once

#include "interception/IPacketHandler.h"
#include "persistence/ConnectionTable.h"
#include "diagnostics/ILogger.h"

namespace Proxirae {
	class UdpHandler : public IPacketHandler {
	public:
		UdpHandler(std::uint16_t redirectPort, ConnectionTable& connections, ILogger& logger);

		bool CanHandle(const IPacketContext& ctx) override;
		void Handle(PacketDispatchContext& ctx) override;

	private:
		ConnectionTable& m_connections;
		ILogger& m_logger;
		std::uint16_t m_redirectPort;
	};
}