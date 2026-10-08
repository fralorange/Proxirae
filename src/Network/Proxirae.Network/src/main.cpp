#include <thread>
#include <future>

#include "features/persistence/connections/ConnectionTable.h"
#include "features/persistence/associations/AssociationTable.h"
#include "features/persistence/virtuals/VirtualTable.h"
#include "features/diagnostics/journal/JournalLogger.h"
#include "orchestration/Engine.h"
#include "orchestration/Daemon.h"
#include "orchestration/Host.h"
#include "features/interception/handling/TcpHandler.h"
#include "features/interception/handling/UdpHandler.h"
#include "features/transport/stream/TcpListener.h"
#include "features/persistence/configuration/ConfigurationLoader.h"
#include "features/persistence/preferences/PreferencesLoader.h"
#include "features/proxification/ProxyFactory.h"
#include "features/communication/channels/IpcChannel.h"
#include "utils/PathUtils.h"
#include "features/communication/channels/handlers/configuration/ConfigurationPipeHandler.h"
#include "features/communication/channels/handlers/preferences/PreferencesPipeHandler.h"
#include "features/communication/channels/handlers/flow/FlowPipeHandler.h"
#include "features/communication/channels/handlers/test/TestPipeHandler.h"
#include "features/communication/channels/messages/PipeMessageType.h"
#include "features/communication/monitoring/Monitor.h"
#include "features/interception/correlation/DispatcherCorrelator.h"
#include "features/interception/handling/DispatcherHandler.h"
#include "features/transport/stream/TcpMultiplexer.h"
#include "features/transport/datagram/UdpMultiplexer.h"
#include "features/transport/datagram/UdpBinder.h"
#include "features/transport/DispatcherMultiplexer.h"

#ifdef _WIN32
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")

#include "asyncio/async/win/IocpDriver.h"
#include "features/interception/diversion/win/WinPacketDiverter.h"
#include "features/interception/correlation/win/WinTcpCorrelator.h"
#include "features/interception/correlation/win/WinUdpCorrelator.h"
#include "platform/processes/resolver/win/WinProcessResolver.h"
#include "platform/processes/manager/win/WinProcessManager.h"
#include "features/communication/pipes/win/WinPipeServer.h"
#include "asyncio/io/stream/win/IocpStreamAdapter.h"
#include "asyncio/io/datagram/win/IocpDatagramAdapter.h"
#include "platform/protection/win/WinProtector.h"
#include "features/interception/correlation/win/WinUdpLookupTable.h"
#endif

using namespace Proxirae;

int main()
{
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
        logger.LogCritical(
            std::format(
                "[Winsock] WSAStartup failed: error {}",
                WSAGetLastError()
            )
        );

        return 1;
    }

    WinProcessResolver processResolver;
    WinProcessManager processManager;

    auto lookupTable = WinUdpLookupTable::TryCreate();

    WinTcpCorrelator tcpCorrelator(
        processResolver,
        associations
    );

    correlator.Register(tcpCorrelator);
#endif

    ConnectionTable connections;
    VirtualTable virtuals;

    RuleEvaluator evaluator;

    Monitor monitor(messenger);

    DispatcherHandler handler;
    PacketRouter router(
        processResolver,
        evaluator,
        configStore,
        connections,
        monitor
    );

    if (!driver.Start(6)) {
        logger.LogCritical(
            "[AsyncDriver] Failed to start IO Driver (6 worker threads)"
        );

        return 1;
    }

    ProxyFactory factory(
        configStore,
        driver,
        streamAdapter,
        datagramAdapter,
        logger
    );

    TcpListener tcpListener(
        driver,
        streamAdapter,
        logger,
        factory
    );

    std::uint16_t tcpPort = tcpListener.Bind();

    if (tcpPort == 0) {
        logger.LogCritical(
            "[Main] Failed to bind redirect TCP listener port"
        );

        return -1;
    }

    UdpBinder udpBinder(driver, logger);

    std::uint16_t udpPort = udpBinder.Bind();

    if (udpPort == 0) {
        logger.LogCritical(
            "[Main] Failed to bind redirect UDP binder port"
        );

        return -1;
    }

#ifdef _WIN32
    WinUdpCorrelator udpCorrelator(
        processResolver,
        associations,
        std::move(lookupTable),
        udpPort
    );

    correlator.Register(udpCorrelator);

    WinPacketDiverter diverter(
        correlator,
        configStore,
        logger
    );
#endif

    TcpHandler tcpHandler(
        tcpPort,
        connections,
        logger
    );

    UdpHandler udpHandler(
        udpPort,
        connections,
        virtuals,
        logger
    );

    handler.RegisterHandler(tcpHandler);
    handler.RegisterHandler(udpHandler);

    TcpMultiplexer tcpMultiplexer(
        tcpListener,
        connections,
        monitor,
        logger,
        tcpPort
    );

    UdpMultiplexer udpMultiplexer(
        udpBinder,
        datagramAdapter,
        connections,
        virtuals,
        monitor,
        factory,
        logger
    );

    DispatcherMultiplexer multiplexer;

    multiplexer.Register(tcpMultiplexer);
    multiplexer.Register(udpMultiplexer);

    Engine engine(
        diverter,
        handler,
        router,
        stopSource.get_token()
    );

    Daemon daemon(
        multiplexer,
        logger,
        monitor,
        stopSource.get_token()
    );

    TestableProxyFactory testableProxyFactory(
        driver,
        streamAdapter,
        logger
    );

    SessionController sessionController(
        daemon,
        processManager
    );

    TestController testController(
        messenger,
        testableProxyFactory,
        logger
    );

    ConfigurationPipeHandler confHandler(
        configLoader,
        diverter
    );

    PreferencesPipeHandler prefHandler(
        prefsLoader
    );

    FlowPipeHandler flowHandler(
        sessionController
    );

    TestPipeHandler testHandler(
        testController,
        protector
    );

    channel.RegisterHandler(
        PipeMessageType::Cmd_ReloadProxies,
        confHandler
    );

    channel.RegisterHandler(
        PipeMessageType::Cmd_ReloadRules,
        confHandler
    );

    channel.RegisterHandler(
        PipeMessageType::Cmd_ReloadPreferences,
        prefHandler
    );

    channel.RegisterHandler(
        PipeMessageType::Cmd_DisconnectFlow,
        flowHandler
    );

    channel.RegisterHandler(
        PipeMessageType::Cmd_EndFlowProcess,
        flowHandler
    );

    channel.RegisterHandler(
        PipeMessageType::Cmd_CheckProxy,
        testHandler
    );

    Host host(
        channel,
        channel,
        stopSource
    );

    std::promise<bool> listenPromise;
    std::future<bool> listenFuture = listenPromise.get_future();

    std::thread daemonThread(
        [&daemon, &listenPromise]() {
            daemon.Run(
                [&listenPromise](bool success) {
                    listenPromise.set_value(success);
                }
            );
        }
    );

    if (!listenFuture.get()) {
        logger.LogError(
            "[Main] Daemon failed to start background listener thread"
        );

        return 1;
    }

    std::thread hostThread(
        [&host]() {
            host.Run();
        }
    );

    std::thread monitorThread(
        [&monitor, &daemon, &stopSource]() {
            auto token = stopSource.get_token();

            while (!token.stop_requested()) {
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(1000)
                );

                auto snapshot = daemon.GetActiveFlows();

                if (snapshot.has_value()) {
                    monitor.ReportFlowSnapshot(*snapshot);
                }
            }
        }
    );

    engine.Run();

    daemonThread.join();
    hostThread.join();
    monitorThread.join();

#ifdef _WIN32
    WSACleanup();
#endif

    return 0;
}