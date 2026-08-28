#pragma once

#include "ipc/IPipeMessageHandler.h"
#include "persistence/PreferencesLoader.h"

namespace Proxirae {
	class PreferencesPipeHandler : public IPipeMessageHandler {
	public:
		PreferencesPipeHandler(PreferencesLoader& loader);

		void Handle(const PipeMessage& msg) override;

	private:
		PreferencesLoader& m_loader;
	};
}