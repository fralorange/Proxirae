#pragma once

#include "communication/IPipeMessageHandler.h"
#include "persistence/ConfigurationLoader.h"

namespace Proxirae {
	class ConfigurationPipeHandler : public IPipeMessageHandler {
	public:
		ConfigurationPipeHandler(ConfigurationLoader& loader);

		void Handle(const PipeMessage& msg) override;

	private:
		ConfigurationLoader& m_loader;
	};
}