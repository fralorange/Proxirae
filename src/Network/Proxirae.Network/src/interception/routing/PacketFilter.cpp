#include <sstream>

#include "interception/routing/PacketFilter.h"

namespace Proxirae {
	std::string PacketFilter::BuildNetworkFilter(std::shared_ptr<const Configuration> config)
	{
		std::ostringstream filter;

		const std::string tcpExclusions =
			" and tcp.SrcPort != 139 and tcp.DstPort != 139"
			" and tcp.SrcPort != 445 and tcp.DstPort != 445";

		const std::string udpExclusions =
			" and udp.SrcPort != 67 and udp.DstPort != 67"
			" and udp.SrcPort != 68 and udp.DstPort != 68"
			" and udp.SrcPort != 137 and udp.DstPort != 137"
			" and udp.SrcPort != 138 and udp.DstPort != 138"
			" and udp.SrcPort != 5353 and udp.DstPort != 5353"
			" and udp.SrcPort != 5355 and udp.DstPort != 5355";

		filter << "(";

		filter << "(ip and tcp" << tcpExclusions;
		for (const auto& [_, p] : config->proxies) {
			if (p.address.find(':') == std::string::npos) {
				filter << " and (ip.SrcAddr != " << p.address << " or tcp.SrcPort != " << p.port << ")"
					<< " and (ip.DstAddr != " << p.address << " or tcp.DstPort != " << p.port << ")";
			}
		}
		filter << ")";

		filter << " or ";

		filter << "(ipv6 and tcp" << tcpExclusions;
		for (const auto& [_, p] : config->proxies) {
			if (p.address.find(':') != std::string::npos) {
				filter << " and (ipv6.SrcAddr != " << p.address << " or tcp.SrcPort != " << p.port << ")"
					<< " and (ipv6.DstAddr != " << p.address << " or tcp.DstPort != " << p.port << ")";
			}
		}
		filter << ")";

		filter << " or ";

		filter << "(ip and udp" << udpExclusions;
		for (const auto& [_, p] : config->proxies) {
			if (p.address.find(':') == std::string::npos) {
				filter << " and (ip.SrcAddr != " << p.address << " or udp.SrcPort != " << p.port << ")"
					<< " and (ip.DstAddr != " << p.address << " or udp.DstPort != " << p.port << ")";
			}
		}
		filter << ")";

		filter << " or ";

		filter << "(ipv6 and udp" << udpExclusions;
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