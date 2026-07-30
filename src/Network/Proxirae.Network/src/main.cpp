#include <thread>

#include "registry/ConnectionTable.h"
#include "diagnostics/ConsoleLogger.h"
#include "core/Engine.h"
#include "core/Daemon.h"
#include "core/TcpHandler.h"

using namespace Proxirae;

int main() {
	ConsoleLogger logger;

	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 2);

	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		logger.LogCritical("Failed to initialize Winsock.");
		return 1;
	}

	ConnectionTable connections;

	PacketDispatcher dispatcher;
	PacketDiverter diverter(logger);

	TcpHandler tcpHandler(connections, logger);
	dispatcher.RegisterHandler(tcpHandler);

	TcpListener listener(logger);

	Engine engine(diverter, dispatcher);
	Daemon daemon(listener, connections, logger);

	std::thread daemonThread([&daemon]() {
		daemon.Start();
	});

	engine.Run();

	daemon.Stop();
	daemonThread.join();

	WSACleanup();

	return 0;
}