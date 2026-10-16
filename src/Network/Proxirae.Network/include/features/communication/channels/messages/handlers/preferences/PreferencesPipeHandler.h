#pragma once

#include "features/communication/channels/messages/handlers/SafePipeMessageHandler.h"
#include "features/persistence/preferences/PreferencesLoader.h"

namespace Proxirae {
	class PreferencesPipeHandler : public SafePipeMessageHandler {
	public:
		PreferencesPipeHandler(PreferencesLoader& loader, ILogger& logger);

	protected:
		void HandleUnsafe(const PipeMessage& msg) override;

	private:
		PreferencesLoader& m_loader;
	};
}