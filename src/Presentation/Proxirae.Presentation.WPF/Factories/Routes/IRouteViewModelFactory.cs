using Proxirae.Contracts.DTOs.Routes;
using Proxirae.Presentation.WPF.ViewModels.Routes;

namespace Proxirae.Presentation.WPF.Factories.Routes
{
    public interface IRouteViewModelFactory
    {
        Task<RouteViewModel?> CreateAsync(RouteDto dto, CancellationToken cancellationToken);
    }
}
