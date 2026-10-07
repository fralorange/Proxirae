#pragma once

#include "features/communication/channels/handlers/IPipeMessageHandler.h"
#include "features/persistence/preferences/PreferencesLoader.h"

namespace Proxirae {
	class PreferencesPipeHandler : public IPipeMessageHandler {
	public:
		PreferencesPipeHandler(PreferencesLoader& loader);

		void Handle(const PipeMessage& msg) override;

	private:
		PreferencesLoader& m_loader;
	};
}