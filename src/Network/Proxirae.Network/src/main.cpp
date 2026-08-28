#include <thread>
#include <future>

#include "persistence/ConnectionTable.h"
#include "persistence/AssociationTable.h"
#include "diagnostics/JournalLogger.h"
#include "application/Engine.h"
#include "application/Daemon.h"
#include "application/Host.h"
#include "packet/TcpHandler.h"
#include "transport/TcpListener.h"
#include "persistence/ConfigurationLoader.h"
#include "persistence/PreferencesLoader.h"
#include "proxy/ProxyFactory.h"
#include "ipc/IpcChannel.h"
#include "utils/PathUtils.h"
#include "ipc/ConfigurationPipeHandler.h"
#include "ipc/PreferencesPipeHandler.h"
#include "ipc/FlowPipeHandler.h"
#include "ipc/TestPipeHandler.h"
#include "ipc/PipeMessageType.h"
#include "monitoring/Monitor.h"

#ifdef _WIN32
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")
#include "io/win/IocpDriver.h"
#include "packet/win/PacketWinDiverter.h"
#include "process/win/WinProcessResolver.h"
#include "process/win/WinProcessManager.h"
#include "ipc/win/WinPipeServer.h"
#endif 

using namespace Proxirae;

int main() {
	std::stop_source stopSource;

	Configuration config;

	auto configPtr = std::make_shared<const Configuration>(std::move(config));
	Store configStore(configPtr);

	Preferences prefs;

	auto prefsPtr = std::make_shared<const Preferences>(std::move(prefs));
	Store prefsStore(prefsPtr);

#ifdef _WIN32
	IocpDriver driver;
	WinPipeServer pipe(driver);
#endif 
	IpcChannel channel(pipe);
	IpcMessenger messenger(channel);

	JournalLogger logger(messenger, prefsStore);

	auto appDir = PathUtils::GetAppConfigDirectory();

	ConfigurationLoader configLoader(configStore, appDir, logger);
	PreferencesLoader prefsLoader(prefsStore, appDir, logger);

	configLoader.Load();
	prefsLoader.Load();

	AssociationTable associations;
	
#ifdef _WIN32
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 2);

	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		logger.LogCritical(std::format("[Winsock] WSAStartup failed: error {}", WSAGetLastError()));
		return 1;
	}
	
	WinProcessResolver processResolver;
	WinProcessManager processManager;
	WinTcpCorrelator tcpCorrelator(processResolver, associations);
	PacketWinDiverter diverter(tcpCorrelator, logger);
#endif 
	ConnectionTable connections;

	RuleEvaluator evaluator;

	Monitor monitor(messenger);

	PacketDispatcher dispatcher;
	PacketRouter router(processResolver, evaluator, configStore, connections, monitor);

	if (!driver.Start(6)) {
		logger.LogCritical("[IocpDriver] Failed to start IO Driver (6 worker threads)");
		return 1;
	}

	ProxyFactory factory(configStore, driver, logger);

	TcpListener listener(driver, logger, factory);
	std::uint16_t port = listener.Bind();

	if (port == 0) {
		logger.LogCritical("[Main] Failed to bind redirect TCP listener port");
		return -1;
	}

	TcpHandler tcpHandler(port, connections, logger);
	dispatcher.RegisterHandler(tcpHandler);

	Engine engine(diverter, dispatcher, router, stopSource.get_token());
	Daemon daemon(listener, connections, logger, monitor, stopSource.get_token());

	SessionController sessionController(daemon, processManager);
	TestController testController(messenger, driver, logger);

	ConfigurationPipeHandler confHandler(configLoader);
	PreferencesPipeHandler prefHandler(prefsLoader);
	FlowPipeHandler flowHandler(sessionController);
	TestPipeHandler testHandler(testController);

	channel.RegisterHandler(PipeMessageType::Cmd_ReloadProxies, confHandler);
	channel.RegisterHandler(PipeMessageType::Cmd_ReloadRules, confHandler);
	channel.RegisterHandler(PipeMessageType::Cmd_ReloadPreferences, prefHandler);
	channel.RegisterHandler(PipeMessageType::Cmd_DisconnectFlow, flowHandler);
	channel.RegisterHandler(PipeMessageType::Cmd_EndFlowProcess, flowHandler);
	channel.RegisterHandler(PipeMessageType::Cmd_CheckProxy, testHandler);

	Host host(channel, channel, stopSource);

	std::promise<bool> listenPromise;
	std::future<bool> listenFuture = listenPromise.get_future();

	std::thread daemonThread([&daemon, port, &listenPromise]() {
		daemon.Run(port, [&listenPromise](bool success) {
			listenPromise.set_value(success);
		});
	});

	if (!listenFuture.get()) {
		logger.LogError("[Main] Daemon failed to start background listener thread");
		return 1;
	}

	std::thread hostThread([&host]() {
		host.Run();
	});

	std::thread monitorThread([&monitor, &daemon, &stopSource]() {
		auto token = stopSource.get_token();

		while (!token.stop_requested()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));

			auto snapshot = daemon.GetActiveFlows();
			if (snapshot.has_value()) {
				monitor.ReportFlowSnapshot(*snapshot);
			}
 		}
	});

	engine.Run();

	daemonThread.join();
	hostThread.join();
	monitorThread.join();

#ifdef _WIN32
	WSACleanup();
#endif

	return 0;
}