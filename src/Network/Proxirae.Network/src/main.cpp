#include <thread>
#include <future>

#include "persistence/ConnectionTable.h"
#include "persistence/AssociationTable.h"
#include "diagnostics/ConsoleLogger.h"
#include "application/Engine.h"
#include "application/Daemon.h"
#include "packet/TcpHandler.h"
#include "transport/TcpListener.h"
#include "persistence/Configuration.h"
#include "persistence/ConfigurationStore.h"
#include "proxy/ProxyFactory.h"

#ifdef _WIN32
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")
#include "io/win/IocpDriver.h"
#include "packet/win/PacketWinDiverter.h"
#include "process/win/WinProcessResolver.h"
#endif 

using namespace Proxirae;

int main() {
	Configuration configuration;

	auto configurationPtr = std::make_shared<const Configuration>(std::move(configuration));
	ConfigurationStore config(configurationPtr);

	ConsoleLogger logger;

	AssociationTable associations;
	
#ifdef _WIN32
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 2);

	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		logger.LogCritical("Failed to initialize Winsock.");
		return 1;
	}
	
	WinProcessResolver processResolver;
	WinTcpCorrelator tcpCorrelator(processResolver, associations);
	PacketWinDiverter diverter(tcpCorrelator, logger);

	IocpDriver driver;
#endif 

	ConnectionTable connections;

	RuleEvaluator evaluator;

	PacketDispatcher dispatcher;
	PacketRouter router(processResolver, evaluator, config, connections);

	if (!driver.Start(4)) {
		logger.LogCritical("Failed to start IO Driver");
		return 1;
	}

	ProxyFactory factory(config, driver, logger);

	TcpListener listener(driver, logger, factory);
	std::uint16_t port = listener.Bind();

	if (port == 0) {
		return -1;
	}

	TcpHandler tcpHandler(port, connections, logger);
	dispatcher.RegisterHandler(tcpHandler);

	Engine engine(diverter, dispatcher, router);
	Daemon daemon(listener, connections, logger);

	std::promise<bool> listenPromise;
	std::future<bool> listenFuture = listenPromise.get_future();

	std::thread daemonThread([&daemon, port, &listenPromise]() {
		daemon.Run(port, [&listenPromise](bool success) {
			listenPromise.set_value(success);
		});
	});

	if (!listenFuture.get()) {
		return 1;
	}

	engine.Run();

	daemonThread.join();

#ifdef _WIN32
	WSACleanup();
#endif

	return 0;
}