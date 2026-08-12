#include <thread>
#include <future>

#include "core/registry/ConnectionRegistry.h"
#include "core/registry/AssociationRegistry.h"
#include "diagnostics/ConsoleLogger.h"
#include "core/Engine.h"
#include "core/Daemon.h"
#include "core/packet/TcpHandler.h"
#include "core/transport/TcpListener.h"
#include "contracts/proxy/ProxyContract.h"
#include "core/proxy/ProxyFactory.h"

#ifdef _WIN32
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")
#include "core/io/win/IocpDriver.h"
#include "core/packet/win/PacketWinDiverter.h"
#include "process/win/WinProcessResolver.h"
#endif 

using namespace Proxirae;

int main() {
	ProxyContract proxy{
		.id = "00000000-0000-0000-0000-000000000000",
		.address = "127.0.0.1",
		.port = 10808,
		.type = ProxyType::SOCKS5,
	};

	std::unordered_map<std::string, ProxyContract> proxies;

	RuleContract rule{
		.priority = 10,
		.isEnabled = true,
		.hosts = "188.40.167.82",
		.protocol = RuleProtocol::TCP,
		.action = RuleActionContract{.action = RuleAction::Proxy, .proxyId = "00000000-0000-0000-0000-000000000000"},
	};

	std::set<RuleContract> rules;

	rules.emplace(rule);
	proxies.emplace(proxy.id, proxy);

	ConsoleLogger logger;

	AssociationRegistry associations;
	
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

	ConnectionRegistry connections;

	RuleEvaluator evaluator;

	PacketDispatcher dispatcher;
	PacketRouter router(processResolver, evaluator, rules, connections);

	if (!driver.Start(4)) {
		logger.LogCritical("Failed to start IO Driver");
		return 1;
	}

	ProxyFactory factory(proxies, driver, logger);

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