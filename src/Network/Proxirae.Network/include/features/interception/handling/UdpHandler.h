#pragma once

#include "features/interception/handling/IPacketHandler.h"
#include "features/persistence/connections/ConnectionTable.h"
#include "features/persistence/virtuals/VirtualTable.h"
#include "features/diagnostics/ILogger.h"

namespace Proxirae {
	class UdpHandler : public IPacketHandler {
	public:
		UdpHandler(std::uint16_t redirectPort, ConnectionTable& connections, VirtualTable& virtuals, ILogger& logger);

		bool Handle(HandleContext& ctx) override;

	private:
		ConnectionTable& m_connections;
		VirtualTable& m_virtuals;
		ILogger& m_logger;
		std::uint16_t m_redirectPort;
	};
}