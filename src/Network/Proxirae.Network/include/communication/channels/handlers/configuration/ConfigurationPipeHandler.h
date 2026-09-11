#pragma once

#include "communication/channels/handlers/IPipeMessageHandler.h"
#include "interception/diversion/IPacketDiverter.h"
#include "persistence/configuration/ConfigurationLoader.h"

namespace Proxirae {
	class ConfigurationPipeHandler : public IPipeMessageHandler {
	public:
		ConfigurationPipeHandler(ConfigurationLoader& loader, IPacketDiverter& diverter);

		void Handle(const PipeMessage& msg) override;

	private:
		ConfigurationLoader& m_loader;
		IPacketDiverter& m_diverter;
	};
}