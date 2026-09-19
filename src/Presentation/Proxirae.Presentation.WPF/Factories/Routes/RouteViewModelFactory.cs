using Proxirae.Application.Facades.Routes;
using Proxirae.Contracts.DTOs.Routes;
using Proxirae.Presentation.WPF.Services.Process;
using Proxirae.Presentation.WPF.ViewModels.Routes;

namespace Proxirae.Presentation.WPF.Factories.Routes
{
    public class RouteViewModelFactory : IRouteViewModelFactory
    {
        private readonly RouteFacade _routeFacade;
        private readonly IProcessInfoService _processInfoService;

        public RouteViewModelFactory(RouteFacade routeFacade, IProcessInfoService processInfoService)
        {
            _routeFacade = routeFacade;
            _processInfoService = processInfoService;
        }

        public async Task<RouteViewModel?> CreateAsync(RouteDto dto, CancellationToken cancellationToken)
        {
            var actionName = await _routeFacade.GetActionNameAsync(dto.RuleId, cancellationToken);
            var processInfo = await _processInfoService.GetProcessInfoAsync(dto.ProcessId, cancellationToken);

            if (string.IsNullOrEmpty(actionName) || processInfo is null)
            {
                return null;
            }

            return new RouteViewModel(dto, processInfo.Name, actionName);
        }
    }
}
