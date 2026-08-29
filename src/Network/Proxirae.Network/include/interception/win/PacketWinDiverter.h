#pragma once

#include "interception/IPacketDiverter.h"
#include "diagnostics/ILogger.h"
#include "interception/win/WinTcpCorrelator.h"
#include "persistence/Configuration.h"
#include "persistence/Store.h"

namespace Proxirae {
	class PacketWinDiverter : public IPacketDiverter {
	public:
		PacketWinDiverter(WinTcpCorrelator& tcpCorrelator, Store<Configuration>& store, ILogger& logger);
		~PacketWinDiverter();

		bool Open() override;
		void Close() override;

		void Reload() override;
		void Interrupt() override;

		bool Receive(const std::function<void(IPacketContext&)>& callback) override;

		bool Send(IPacketContext& ctx) override;

	private:
		class DivertChannel;

		HANDLE m_reload;

		std::unique_ptr<DivertChannel> m_network;
		std::unique_ptr<DivertChannel> m_socket;

		WinTcpCorrelator& m_tcpCorrelator;
		Store<Configuration>& m_store;
		ILogger& m_logger;
	};
}