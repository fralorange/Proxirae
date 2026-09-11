#include <sstream>

#include "interception/routing/PacketFilter.h"

namespace Proxirae {
	std::string PacketFilter::BuildNetworkFilter(std::shared_ptr<const Configuration> config)
	{
		std::ostringstream filter;

		filter << "ip and (";

		filter << "(tcp";
		for (const auto& [_, p] : config->proxies) {
			filter << " and (ip.SrcAddr != " << p.address << " or tcp.SrcPort != " << p.port << ")"
				<< " and (ip.DstAddr != " << p.address << " or tcp.DstPort != " << p.port << ")";
		}
		filter << ")";

		filter << " or ";

		filter << "(udp";
		for (const auto& [_, p] : config->proxies) {
			filter << " and (ip.SrcAddr != " << p.address << " or udp.SrcPort != " << p.port << ")"
				<< " and (ip.DstAddr != " << p.address << " or udp.DstPort != " << p.port << ")";
		}
		filter << ")";

		filter << ")";

		return filter.str();
	}

	std::string PacketFilter::BuildSocketFilter()
	{
		return "(tcp or udp) and (event == BIND or event == CONNECT or event == CLOSE)";
	}
}