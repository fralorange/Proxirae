using Proxirae.Application.Services.Flows;
using Proxirae.Contracts.DTOs.Flows;
using System.Text.Json;

namespace Proxirae.Infrastructure.ProcessCommunication.Pipes.Message.Handlers.Flows
{
    public class FlowMessageHandler : IPipeMessageHandler
    {
        private readonly IFlowService _flowService;

        public FlowMessageHandler(IFlowService flowService)
        {
            _flowService = flowService;
        }

        public Task HandleAsync(PipeMessage message, CancellationToken cancellationToken)
        {
            var flows = JsonSerializer.Deserialize<List<FlowDto>>(message.Payload);

            if (flows is not null)
            {
                if (flows.Count == 1 && flows.FirstOrDefault() is { Status: FlowStatusDto.Closed } flow)
                {
                    _flowService.NotifyFlowClosed(flow);
                } else
                {
                    _flowService.NotifyFlowsUpdated(flows);
                }
            }

            return Task.CompletedTask;
        }
    }
}
