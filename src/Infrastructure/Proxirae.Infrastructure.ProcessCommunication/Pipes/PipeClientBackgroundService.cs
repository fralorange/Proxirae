using Microsoft.Extensions.Hosting;
using Proxirae.Application.Services.Application;
using Proxirae.Infrastructure.ProcessCommunication.Channels;

namespace Proxirae.Infrastructure.ProcessCommunication.Pipes
{
    public sealed class PipeClientBackgroundService : BackgroundService
    {
        private readonly IConnectChannel _connector;
        private readonly IReceiveChannel _receiver;
        private readonly IDispatchChannel _dispatcher;
        private readonly IApplicationService _applicationService;

        public PipeClientBackgroundService(
            IConnectChannel connector,
            IReceiveChannel receiver,
            IDispatchChannel dispatcher,
            IApplicationService applicationService)
        {
            _connector = connector;
            _receiver = receiver;
            _dispatcher = dispatcher;
            _applicationService = applicationService;
        }

        protected override async Task ExecuteAsync(CancellationToken cancellationToken)
        {
            await _connector.ConnectAsync(cancellationToken);

            try
            {
                while (!cancellationToken.IsCancellationRequested)
                {
                    var message = await _receiver.ReceiveAsync(cancellationToken);

                    if (message != null)
                    {
                        await _dispatcher.DispatchAsync(message, cancellationToken);
                    }
                    else
                    {
                        _applicationService.Shutdown();
                        break;
                    }
                }
            }
            finally
            {
                await _connector.DisconnectAsync(cancellationToken);
            }
        }
    }
}
