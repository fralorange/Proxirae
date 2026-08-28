using Proxirae.Contracts.DTOs.Routes;

namespace Proxirae.Application.Services.Routes
{
    public class RouteService : IRouteService
    {
        public event Action<RouteDto>? RouteReceived;

        public void NotifyRouteReceived(RouteDto routeDto)
        {
            RouteReceived?.Invoke(routeDto);
        }
    }
}
