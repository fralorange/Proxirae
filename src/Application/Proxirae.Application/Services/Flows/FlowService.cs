using Proxirae.Application.Messenger;
using Proxirae.Contracts.DTOs.Flows;

namespace Proxirae.Application.Services.Flows
{
    public class FlowService : IFlowService
    {
        private readonly IMessenger _messenger;

        public event Action<IEnumerable<FlowDto>>? FlowsUpdated;
        public event Action<FlowDto>? FlowClosed;

        public FlowService(IMessenger messenger)
        {
            _messenger = messenger;
        }

        public void NotifyFlowsUpdated(IEnumerable<FlowDto> flows)
        {
            FlowsUpdated?.Invoke(flows);
        }

        public void NotifyFlowClosed(FlowDto flow)
        {
            FlowClosed?.Invoke(flow);
        }

        public Task DisconnectFlowAsync(Guid id, CancellationToken cancellationToken)
        {
            var dto = new FlowDisconnectDto { Id = id };   

            return _messenger.SendMessageAsync(MessageType.Cmd_DisconnectFlow, dto, cancellationToken);
        }

        public Task EndFlowProcessAsync(long processId, CancellationToken cancellationToken)
        {
            if (processId < 0) processId = 0;

            var dto = new FlowDestroyDto { ProcessId = (uint)processId };

            return _messenger.SendMessageAsync(MessageType.Cmd_EndFlowProcess, dto, cancellationToken);
        }
    }
}
