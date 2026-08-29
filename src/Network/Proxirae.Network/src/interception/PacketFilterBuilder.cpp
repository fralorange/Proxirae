#include <sstream>

#include "interception/PacketFilterBuilder.h"

namespace Proxirae {
	std::string PacketFilterBuilder::BuildNetworkFilter(std::shared_ptr<const Configuration> config)
	{
		std::ostringstream filter;

		filter << "ip and tcp";

		for (const auto& proxy : config->proxies) {
			filter << " and tcp.SrcPort != " << proxy.second.port
				   << " and tcp.DstPort != " << proxy.second.port;
		}

		return filter.str();
	}

	std::string PacketFilterBuilder::BuildSocketFilter()
	{
		return "tcp and (event == CONNECT or event == CLOSE)";
	}
}