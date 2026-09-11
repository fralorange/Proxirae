using Proxirae.Contracts.DTOs.Flows;
using Proxirae.Presentation.WPF.ViewModels.Flows;

namespace Proxirae.Presentation.WPF.Factories.Flow
{
    public interface IFlowViewModelFactory
    {
        Task<FlowViewModel?> CreateAsync(FlowDto dto, CancellationToken cancellationToken);
    }
}
