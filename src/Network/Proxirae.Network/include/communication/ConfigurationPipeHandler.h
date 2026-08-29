#pragma once

#include "communication/IPipeMessageHandler.h"
#include "interception/IPacketDiverter.h"
#include "persistence/ConfigurationLoader.h"

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