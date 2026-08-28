
using Proxirae.Application.Services.Routes;
using Proxirae.Contracts.DTOs.Routes;
using System.Text.Json;

namespace Proxirae.Infrastructure.ProcessCommunication.Pipes.Message.Handlers.Routes
{
    public class RouteMessageHandler : IPipeMessageHandler
    {
        private readonly IRouteService _routeService;

        public RouteMessageHandler(IRouteService routeService)
        {
            _routeService = routeService;
        }

        public Task HandleAsync(PipeMessage message, CancellationToken cancellationToken)
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
