#include <thread>
#include <future>

#include "core/registry/ConnectionTable.h"
#include "diagnostics/ConsoleLogger.h"
#include "core/Engine.h"
#include "core/Daemon.h"
#include "core/packet/TcpHandler.h"
#include "core/transport/TcpListener.h"

#ifdef _WIN32
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")
#include "core/io/win/IocpDriver.h"
#include "core/packet/win/PacketWinDiverter.h"
#endif 

using namespace Proxirae;

int main() {
	ConsoleLogger logger;
	
#ifdef _WIN32
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 2);

	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		logger.LogCritical("Failed to initialize Winsock.");
		return 1;
	}

	PacketWinDiverter diverter(logger);
	IocpDriver driver;
#endif 

	ConnectionTable connections;

	PacketDispatcher dispatcher;

	if (!driver.Start(4)) {
		logger.LogCritical("Failed to start IO Driver");
		return 1;
	}

	TcpListener listener(driver, logger);
	std::uint16_t port = listener.Bind();

	if (port == 0) {
		return -1;
	}

	TcpHandler tcpHandler(port, connections, logger);
	dispatcher.RegisterHandler(tcpHandler);

	Engine engine(diverter, dispatcher);
	Daemon daemon(listener, connections, logger);

	std::promise<bool> listenPromise;
	std::future<bool> listenFuture = listenPromise.get_future();

	std::thread daemonThread([&daemon, port, &listenPromise]() {
		daemon.Start(port, [&listenPromise](bool success) {
			listenPromise.set_value(success);
		});
	});

	if (!listenFuture.get()) {
		return 1;
	}

	engine.Run();

	daemon.Stop();
	daemonThread.join();

#ifdef _WIN32
	WSACleanup();
#endif

	return 0;
}