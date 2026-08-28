#pragma once

#include "interception/IPacketDiverter.h"
#include "diagnostics/ILogger.h"
#include "interception/win/WinTcpCorrelator.h"

namespace Proxirae {
	class PacketWinDiverter : public IPacketDiverter {
	public:
		PacketWinDiverter(WinTcpCorrelator& tcpCorrelator, ILogger& logger);
		~PacketWinDiverter();

		bool Open() override;
		void Close() override;

		void Interrupt() override;

		bool Receive(const std::function<void(IPacketContext&)>& callback) override;

		bool Send(IPacketContext& ctx) override;

	private:
		class DivertChannel;

		std::unique_ptr<DivertChannel> m_network;
		std::unique_ptr<DivertChannel> m_socket;

		WinTcpCorrelator& m_tcpCorrelator;
		ILogger& m_logger;
	};
}