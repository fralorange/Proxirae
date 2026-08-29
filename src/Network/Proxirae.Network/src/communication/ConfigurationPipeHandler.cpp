#include "communication/ConfigurationPipeHandler.h"
#include "communication/PipeMessageType.h"

namespace Proxirae {
	ConfigurationPipeHandler::ConfigurationPipeHandler(ConfigurationLoader& loader, IPacketDiverter& diverter)
		: m_loader(loader), m_diverter(diverter) { }

	void ConfigurationPipeHandler::Handle(const PipeMessage& msg)
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