using Proxirae.Application.Services.Routes;
using Proxirae.Contracts.DTOs.Routes;
using System.Text.Json;

namespace Proxirae.Infrastructure.ProcessCommunication.Pipes.Message.Handlers.Routes
{
    public class RouteMessageHandler : SafePipeMessageHandler
    {
        private readonly IRouteService _routeService;

        public RouteMessageHandler(IRouteService routeService)
        {
            _routeService = routeService;
        }

        protected override Task HandleUnsafeAsync(PipeMessage message, CancellationToken cancellationToken)
        {
            var route = JsonSerializer.Deserialize<RouteDto>(message.Payload);

            if (route is not null)
            {
                _routeService.NotifyRouteReceived(route);
            }

            return Task.CompletedTask;
        }
    }
}
