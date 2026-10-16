#pragma once

#include "features/communication/channels/messages/handlers/SafePipeMessageHandler.h"
#include "features/interception/diversion/IPacketDiverter.h"
#include "features/persistence/configuration/ConfigurationLoader.h"

namespace Proxirae {
	class ConfigurationPipeHandler : public SafePipeMessageHandler {
	public:
		ConfigurationPipeHandler(ConfigurationLoader& loader, IPacketDiverter& diverter, ILogger& logger);

	protected:
		void HandleUnsafe(const PipeMessage& msg) override;

	private:
		ConfigurationLoader& m_loader;
		IPacketDiverter& m_diverter;
	};
}