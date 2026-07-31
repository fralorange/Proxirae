#pragma once

#include <vector>
#include <windivert.h>

#include "core/packet/IPacketDiverter.h"
#include "diagnostics/ILogger.h"

namespace Proxirae {
	class PacketWinDiverter : public IPacketDiverter {
	public:
		PacketWinDiverter(ILogger& logger);
		~PacketWinDiverter();

		bool Open() override;
		void Close() override;

		bool Receive(const std::function<void(IPacketContext&)>& handler) override;

		bool Send(IPacketContext& ctx) override;

	private:
		HANDLE m_handle{ INVALID_HANDLE_VALUE };

		std::vector<unsigned char> m_packetBuffer = std::vector<unsigned char>(0xFFFF);
		std::uint32_t m_packetLen{ 0 };
		WINDIVERT_ADDRESS m_packetAddr{};

		ILogger& m_logger;
	};
}