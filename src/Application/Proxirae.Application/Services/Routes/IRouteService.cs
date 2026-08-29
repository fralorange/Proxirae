using Proxirae.Contracts.DTOs.Routes;

namespace Proxirae.Application.Services.Routes
{
    public interface IRouteService
    {
        event Action<RouteDto>? RouteReceived;
        void NotifyRouteReceived(RouteDto routeDto);
    }
}
