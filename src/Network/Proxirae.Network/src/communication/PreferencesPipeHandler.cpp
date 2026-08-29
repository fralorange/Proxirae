#include "communication/PreferencesPipeHandler.h"

namespace Proxirae {
	PreferencesPipeHandler::PreferencesPipeHandler(PreferencesLoader& loader)
		: m_loader(loader) { }

	void PreferencesPipeHandler::Handle(const PipeMessage& msg)
	{
		m_loader.Load();
	}
}