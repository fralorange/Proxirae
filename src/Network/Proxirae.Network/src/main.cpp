#include <thread>
#include <future>

#include "persistence/connections/ConnectionTable.h"
#include "persistence/associations/AssociationTable.h"
#include "diagnostics/journal/JournalLogger.h"
#include "runtime/Engine.h"
#include "runtime/Daemon.h"
#include "runtime/Host.h"
#include "interception/handling/TcpHandler.h"
#include "interception/handling/UdpHandler.h"
#include "transport/stream/TcpListener.h"
#include "persistence/configuration/ConfigurationLoader.h"
#include "persistence/preferences/PreferencesLoader.h"
#include "proxification/ProxyFactory.h"
#include "communication/channels/IpcChannel.h"
#include "utils/PathUtils.h"
#include "communication/channels/handlers/configuration/ConfigurationPipeHandler.h"
#include "communication/channels/handlers/preferences/PreferencesPipeHandler.h"
#include "communication/channels/handlers/flow/FlowPipeHandler.h"
#include "communication/channels/handlers/test/TestPipeHandler.h"
#include "communication/channels/messages/PipeMessageType.h"
#include "monitoring/Monitor.h"
#include "interception/correlation/DispatcherCorrelator.h"
#include "interception/handling/DispatcherHandler.h"
#include "transport/stream/TcpMultiplexer.h"
#include "transport/datagram/UdpMultiplexer.h"
#include "transport/datagram/UdpBinder.h"
#include "transport/DispatcherMultiplexer.h"

#ifdef _WIN32
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")
#include "asyncio/async/win/IocpDriver.h"
#include "interception/diversion/win/WinPacketDiverter.h"
#include "interception/correlation/win/WinTcpCorrelator.h"
#include "interception/correlation/win/WinUdpCorrelator.h"
#include "processes/resolver/win/WinProcessResolver.h"
#include "processes/manager/win/WinProcessManager.h"
#include "communication/pipes/win/WinPipeServer.h"
#include "asyncio/io/stream/win/IocpStreamAdapter.h"
#include "asyncio/io/datagram/win/IocpDatagramAdapter.h"
#include "protection/win/WinProtector.h"
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
	IocpStreamAdapter streamAdapter;
	IocpDatagramAdapter datagramAdapter;
	WinPipeServer pipe(driver, streamAdapter);
	WinProtector protector;
#endif 
	IpcChannel channel(pipe);
	IpcMessenger messenger(channel);

	JournalLogger logger(messenger, prefsStore);

	auto appDir = PathUtils::GetAppConfigDirectory();

	ConfigurationLoader configLoader(configStore, appDir, logger, protector);
	PreferencesLoader prefsLoader(prefsStore, appDir, logger);

	configLoader.Load();
	prefsLoader.Load();

	AssociationTable associations;

	DispatcherCorrelator correlator;
	
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
	WinUdpCorrelator udpCorrelator(processResolver, associations);

	correlator.Register(tcpCorrelator);
	correlator.Register(udpCorrelator);

	WinPacketDiverter diverter(correlator, configStore, logger);
#endif 
	ConnectionTable connections;

	RuleEvaluator evaluator;

	Monitor monitor(messenger);

	DispatcherHandler handler;
	PacketRouter router(processResolver, evaluator, configStore, connections, monitor);

	if (!driver.Start(6)) {
		logger.LogCritical("[IocpDriver] Failed to start IO Driver (6 worker threads)");
		return 1;
	}

	ProxyFactory factory(configStore, driver, streamAdapter, datagramAdapter, logger);

	TcpListener tcpListener(driver, streamAdapter, logger, factory);
	std::uint16_t tcpPort = tcpListener.Bind();

	if (tcpPort == 0) {
		logger.LogCritical("[Main] Failed to bind redirect TCP listener port");
		return -1;
	}

	UdpBinder udpBinder(driver, logger);
	std::uint16_t udpPort = udpBinder.Bind();

	if (udpPort == 0) {
		logger.LogCritical("[Main] Failed to bind redirect UDP binder port");
		return -1;
	}

	TcpHandler tcpHandler(tcpPort, connections, logger);
	UdpHandler udpHandler(udpPort, connections, logger);

	handler.RegisterHandler(tcpHandler);
	handler.RegisterHandler(udpHandler);

	TcpMultiplexer tcpMultiplexer(tcpListener, connections, monitor, logger, tcpPort);
	UdpMultiplexer udpMultiplexer(udpBinder, datagramAdapter, connections, monitor, factory, logger);

	DispatcherMultiplexer multiplexer;
	multiplexer.Register(tcpMultiplexer);
	multiplexer.Register(udpMultiplexer);

	Engine engine(diverter, handler, router, stopSource.get_token());
	Daemon daemon(multiplexer, logger, monitor, stopSource.get_token());

	SessionController sessionController(daemon, processManager);
	TestController testController(messenger, driver, streamAdapter, logger);

	ConfigurationPipeHandler confHandler(configLoader, diverter);
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

	std::thread daemonThread([&daemon, &listenPromise]() {
		daemon.Run([&listenPromise](bool success) {
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