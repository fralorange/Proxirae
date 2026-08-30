#include <sstream>

#include "interception/PacketFilterBuilder.h"

namespace Proxirae {
	std::string PacketFilterBuilder::BuildNetworkFilter(std::shared_ptr<const Configuration> config)
	{
		std::ostringstream filter;

		filter << "ip and tcp and udp";

		for (const auto& proxy : config->proxies) {
			const auto& p = proxy.second;

			filter << " and not (ip.DstAddr == '" << p.address << "' and "
				   << "(tcp.DstPort == " << p.port << " or udp.DstPort == " << p.port << "))";

			filter << " and not (ip.SrcAddr == '" << p.address << "' and "
				   << "(tcp.SrcPort == " << p.port << " or udp.SrcPort == " << p.port << "))";
		}

		return filter.str();
	}

	std::string PacketFilterBuilder::BuildSocketFilter()
	{
		return "tcp and (event == CONNECT or event == CLOSE)";
	}
}