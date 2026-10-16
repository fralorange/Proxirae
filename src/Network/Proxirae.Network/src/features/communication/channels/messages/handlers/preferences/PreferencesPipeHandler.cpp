#include "features/communication/channels/messages/handlers/preferences/PreferencesPipeHandler.h"

namespace Proxirae {
	PreferencesPipeHandler::PreferencesPipeHandler(PreferencesLoader& loader, ILogger& logger)
		: m_loader(loader), SafePipeMessageHandler(logger) { }

	void PreferencesPipeHandler::HandleUnsafe(const PipeMessage& msg)
	{
		m_loader.Load();
	}
}