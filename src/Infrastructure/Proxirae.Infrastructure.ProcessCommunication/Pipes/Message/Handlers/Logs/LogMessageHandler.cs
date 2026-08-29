using Proxirae.Application.Services.Logs;
using Proxirae.Contracts.DTOs.Logs;
using System.Text.Json;

namespace Proxirae.Infrastructure.ProcessCommunication.Pipes.Message.Handlers.Logs
{
    public class LogMessageHandler : IPipeMessageHandler
    {
        private readonly ILogService _logService;

        public LogMessageHandler(ILogService logService)
        {
            _logService = logService;
        }

        public Task HandleAsync(PipeMessage message, CancellationToken cancellationToken)
        {
            var log = JsonSerializer.Deserialize<LogDto>(message.Payload);

            if (log is not null)
            {
                _logService.NotifyLogReceived(log);
            }

            return Task.CompletedTask;
        }
    }
}
