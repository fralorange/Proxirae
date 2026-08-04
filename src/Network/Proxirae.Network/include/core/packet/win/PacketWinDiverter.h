#pragma once

#include <windivert.h>
#include <vector>

#include "core/packet/IPacketDiverter.h"
#include "diagnostics/ILogger.h"
#include "core/packet/win/WinTcpCorrelator.h"

namespace Proxirae {
	class PacketWinDiverter : public IPacketDiverter {
	public:
		PacketWinDiverter(WinTcpCorrelator& tcpCorrelator, ILogger& logger);
		~PacketWinDiverter();

		bool Open() override;
		void Close() override;

		bool Receive(const std::function<void(IPacketContext&)>& callback) override;

		bool Send(IPacketContext& ctx) override;

	private:
		static constexpr std::size_t PACKET_BUFFER_SIZE = 65535;

		HANDLE m_networkHandle{ INVALID_HANDLE_VALUE };
		HANDLE m_networkEvent{ nullptr };

		OVERLAPPED m_networkOverlapped{};

		std::vector<std::uint8_t> m_networkPacketBuffer = std::vector<std::uint8_t>(PACKET_BUFFER_SIZE);
		std::uint32_t m_networkPacketLen{ 0 };
		WINDIVERT_ADDRESS m_networkPacketAddr{};
		std::uint32_t m_networkPacketAddrSize{ sizeof(WINDIVERT_ADDRESS) };

		HANDLE m_socketHandle{ INVALID_HANDLE_VALUE };
		HANDLE m_socketEvent{ nullptr };

		OVERLAPPED m_socketOverlapped{};

		WINDIVERT_ADDRESS m_socketPacketAddr{};
		std::uint32_t m_socketPacketAddrSize{ sizeof(WINDIVERT_ADDRESS) };

		WinTcpCorrelator& m_tcpCorrelator;
		ILogger& m_logger;

		bool OpenNetwork(const char* filter);
		bool OpenSocket(const char* filter);

		static void CloseHandle(HANDLE& handle);

		bool NetworkReceive();
		bool SocketReceive();
	};
}