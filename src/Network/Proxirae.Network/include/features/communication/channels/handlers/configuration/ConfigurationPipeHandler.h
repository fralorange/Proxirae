#pragma once

#include "features/communication/channels/handlers/IPipeMessageHandler.h"
#include "features/interception/diversion/IPacketDiverter.h"
#include "features/persistence/configuration/ConfigurationLoader.h"

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