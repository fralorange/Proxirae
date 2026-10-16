#include "features/communication/channels/messages/handlers/configuration/ConfigurationPipeHandler.h"
#include "features/communication/channels/messages/PipeMessageType.h"

namespace Proxirae {
	ConfigurationPipeHandler::ConfigurationPipeHandler(ConfigurationLoader& loader, IPacketDiverter& diverter, ILogger& logger)
		: m_loader(loader), m_diverter(diverter), SafePipeMessageHandler(logger) { }

	void ConfigurationPipeHandler::HandleUnsafe(const PipeMessage& msg)
	{
		if (msg.header.type == PipeMessageType::Cmd_ReloadProxies) {
			m_loader.Load(LoadTarget::Proxies);
		}
		else {
			m_loader.Load(LoadTarget::Rules);
		}

		m_diverter.Reload();
	}
}