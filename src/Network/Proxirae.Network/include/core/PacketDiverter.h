#pragma once

#include <optional>
#include <vector>
#include <diagnostics/ILogger.h>

#include "Packet.h"

namespace Proxirae {
	class PacketDiverter {
	public:
		PacketDiverter(ILogger& logger);
		~PacketDiverter();

		bool Open();
		void Close();

		std::optional<Packet> Receive();

		bool Send(Packet& packet);

	private:
		HANDLE m_handle{ INVALID_HANDLE_VALUE };

		std::vector<unsigned char> m_packetBuffer = std::vector<unsigned char>(0xFFFF);
		UINT m_packetLen{ 0 };
		WINDIVERT_ADDRESS m_packetAddr{};

		ILogger& m_logger;
	};
}