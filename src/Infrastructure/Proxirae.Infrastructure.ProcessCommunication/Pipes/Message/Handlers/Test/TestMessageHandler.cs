
using Proxirae.Application.Services.Test;
using Proxirae.Contracts.DTOs.Test;
using System.Text.Json;

namespace Proxirae.Infrastructure.ProcessCommunication.Pipes.Message.Handlers.Test
{
    public class TestMessageHandler : IPipeMessageHandler
    {
        private readonly ITestService _testService;

        public TestMessageHandler(ITestService testService)
        {
            _testService = testService;
        }

        public Task HandleAsync(PipeMessage message, CancellationToken cancellationToken)
        {
            var progress = JsonSerializer.Deserialize<TestProgressDto>(message.Payload);

            if (progress is not null)
            {
                _testService.NotifyProgressReceived(progress);
            }

            return Task.CompletedTask;
        }
    }
}
