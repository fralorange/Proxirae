#include <memory>
#include <thread>

#include "Persistence/ConnectionTable.h"
#include "Logging/ConsoleLogger.h"
#include "Engine.h"
#include "Daemon.h"

using namespace Proxirae::Network;

int main() {
	Persistence::ConnectionTable connections;
	Logging::ConsoleLogger logger;

	auto engine = std::make_unique<Engine>(connections, logger);
	auto daemon = std::make_unique<Daemon>(connections, logger);

	std::thread daemonThread([&daemon]() {
		daemon->Start();
	});

	engine->Start();

	daemon->Stop();
	daemonThread.join();

	return 0;
}