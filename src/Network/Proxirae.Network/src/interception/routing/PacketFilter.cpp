#include <sstream>

#include "interception/routing/PacketFilter.h"

namespace Proxirae {
	std::string PacketFilter::BuildNetworkFilter(std::shared_ptr<const Configuration> config)
	{
		std::ostringstream filter;

		filter << "(";

		filter << "(ip and tcp";
		for (const auto& [_, p] : config->proxies) {
			if (p.address.find(':') == std::string::npos) {
				filter << " and (ip.SrcAddr != " << p.address << " or tcp.SrcPort != " << p.port << ")"
					<< " and (ip.DstAddr != " << p.address << " or tcp.DstPort != " << p.port << ")";
			}
		}
		filter << ")";

		filter << " or ";

		filter << "(ipv6 and tcp";
		for (const auto& [_, p] : config->proxies) {
			if (p.address.find(':') != std::string::npos) {
				filter << " and (ipv6.SrcAddr != " << p.address << " or tcp.SrcPort != " << p.port << ")"
					<< " and (ipv6.DstAddr != " << p.address << " or tcp.DstPort != " << p.port << ")";
			}
		}
		filter << ")";

		filter << " or ";

		filter << "(ip and udp";
		for (const auto& [_, p] : config->proxies) {
			if (p.address.find(':') == std::string::npos) {
				filter << " and (ip.SrcAddr != " << p.address << " or udp.SrcPort != " << p.port << ")"
					<< " and (ip.DstAddr != " << p.address << " or udp.DstPort != " << p.port << ")";
			}
		}
		filter << ")";

		filter << " or ";

		filter << "(ipv6 and udp";
		for (const auto& [_, p] : config->proxies) {
			if (p.address.find(':') != std::string::npos) {
				filter << " and (ipv6.SrcAddr != " << p.address << " or udp.SrcPort != " << p.port << ")"
					<< " and (ipv6.DstAddr != " << p.address << " or udp.DstPort != " << p.port << ")";
			}
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