using Proxirae.Application.Facades.Actions;
using Proxirae.Contracts.DTOs.Flows;
using Proxirae.Presentation.WPF.Services.Process;
using Proxirae.Presentation.WPF.ViewModels.Flows;

namespace Proxirae.Presentation.WPF.Factories.Flow
{
    public class FlowViewModelFactory : IFlowViewModelFactory
    {
        private readonly ActionFacade _actionFacade;
        private readonly IProcessInfoService _processInfoService;

        public FlowViewModelFactory(ActionFacade actionFacade, IProcessInfoService processInfoService)
        {
            _actionFacade = actionFacade;
            _processInfoService = processInfoService;
        }

        public async Task<FlowViewModel?> CreateAsync(FlowDto dto, CancellationToken cancellationToken)
        {
            var action = await _actionFacade.GetActionAsync(dto.ProxyId, cancellationToken);
            var processInfo = await _processInfoService.GetProcessInfoAsync(dto.ProcessId, cancellationToken);
            if (processInfo is null) return null;

            return new FlowViewModel(dto, processInfo, action.Name);
        }
    }
}
