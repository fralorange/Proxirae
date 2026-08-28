#include "ipc/ConfigurationPipeHandler.h"
#include "ipc/PipeMessageType.h"

namespace Proxirae {
	ConfigurationPipeHandler::ConfigurationPipeHandler(ConfigurationLoader& loader)
		: m_loader(loader) { }

	void ConfigurationPipeHandler::Handle(const PipeMessage& msg)
	{
		if (msg.header.type == PipeMessageType::Cmd_ReloadProxies) {
			m_loader.Load(LoadTarget::Proxies);
		}
		else {
			m_loader.Load(LoadTarget::Rules);
		}
	}
}