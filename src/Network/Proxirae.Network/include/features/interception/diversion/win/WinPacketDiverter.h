#pragma once

#include "features/interception/diversion/IPacketDiverter.h"
#include "features/diagnostics/ILogger.h"
#include "features/interception/correlation/IPacketCorrelator.h"
#include "features/persistence/configuration/Configuration.h"
#include "features/persistence/Store.h"

namespace Proxirae {
	class WinPacketDiverter : public IPacketDiverter {
	public:
		WinPacketDiverter(IPacketCorrelator& correlator, Store<Configuration>& store, ILogger& logger);
		~WinPacketDiverter();

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

		IPacketCorrelator& m_correlator;
		Store<Configuration>& m_store;
		ILogger& m_logger;
	};
}