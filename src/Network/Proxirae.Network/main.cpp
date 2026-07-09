#include <memory>

#include "TrafficEngine.h"

using namespace Proxirae::Network;

int main() {
	auto trafficEngine = std::make_unique<TrafficEngine>();
	trafficEngine->Start();

    return 0;
}