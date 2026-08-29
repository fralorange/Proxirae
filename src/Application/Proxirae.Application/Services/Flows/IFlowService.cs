using Proxirae.Contracts.DTOs.Flows;

namespace Proxirae.Application.Services.Flows
{
    public interface IFlowService
    {
        event Action<IEnumerable<FlowDto>>? FlowsUpdated;
        event Action<FlowDto>? FlowClosed;
        void NotifyFlowsUpdated(IEnumerable<FlowDto> flows);
        void NotifyFlowClosed(FlowDto flow);
        Task DisconnectFlowAsync(Guid id, CancellationToken cancellationToken);
        Task EndFlowProcessAsync(long processId, CancellationToken cancellationToken);
    }
}
