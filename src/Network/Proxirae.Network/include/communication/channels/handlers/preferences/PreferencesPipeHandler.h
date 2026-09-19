#pragma once

#include "communication/channels/handlers/IPipeMessageHandler.h"
#include "persistence/preferences/PreferencesLoader.h"

namespace Proxirae {
	class PreferencesPipeHandler : public IPipeMessageHandler {
	public:
		PreferencesPipeHandler(PreferencesLoader& loader);

		void Handle(const PipeMessage& msg) override;

	private:
		PreferencesLoader& m_loader;
	};
}