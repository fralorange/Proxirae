using Microsoft.Extensions.DependencyInjection;
using Proxirae.Application.Messenger;
using Proxirae.Application.Pipes;
using Proxirae.Infrastructure.ProcessCommunication.Channels;
using Proxirae.Infrastructure.ProcessCommunication.Pipes;
using Proxirae.Infrastructure.ProcessCommunication.Pipes.Message;
using Proxirae.Infrastructure.ProcessCommunication.Pipes.Message.Handlers.Flows;
using Proxirae.Infrastructure.ProcessCommunication.Pipes.Message.Handlers.Logs;
using Proxirae.Infrastructure.ProcessCommunication.Pipes.Message.Handlers.Routes;
using Proxirae.Infrastructure.ProcessCommunication.Pipes.Message.Handlers.Test;

namespace Proxirae.Infrastructure.ProcessCommunication
{
    public static class DependencyInjection
    {
        public static IServiceCollection AddCommunication(this IServiceCollection services)
        {
            services.AddHostedService<JobProcessHostedService>();

            services.AddSingleton<IPipeClient, PipeClient>();

            services.AddSingleton<IpcChannel>();

            services.AddSingleton<FlowMessageHandler>();
            services.AddSingleton<LogMessageHandler>();
            services.AddSingleton<RouteMessageHandler>();
            services.AddSingleton<TestMessageHandler>();

            services.AddSingleton<IConnectChannel>(sp => sp.GetRequiredService<IpcChannel>());
            services.AddSingleton<ISendChannel>(sp => sp.GetRequiredService<IpcChannel>());
            services.AddSingleton<IReceiveChannel>(sp => sp.GetRequiredService<IpcChannel>());

            services.AddSingleton<IDispatchChannel>(sp =>
            {
                var channel = sp.GetRequiredService<IpcChannel>();

                var flowHandler = sp.GetRequiredService<FlowMessageHandler>();
                var logHandler = sp.GetRequiredService<LogMessageHandler>();
                var routeHandler = sp.GetRequiredService<RouteMessageHandler>();
                var testHandler = sp.GetRequiredService<TestMessageHandler>();

                channel.RegisterHandler((ushort)MessageType.Evt_FlowReceive, flowHandler);
                channel.RegisterHandler((ushort)MessageType.Evt_LogReceive, logHandler);
                channel.RegisterHandler((ushort)MessageType.Evt_RouteReceive, routeHandler);
                channel.RegisterHandler((ushort)MessageType.Evt_ReceiveTestProgress, testHandler);

                return channel;
            });

            services.AddSingleton<IMessenger, PipeMessenger>();

            services.AddHostedService<PipeClientBackgroundService>();

            return services;
        }
    }
}
