using Proxirae.Contracts.DTOs.Logs;

namespace Proxirae.Application.Services.Logs
{
    public class LogService : ILogService
    {
        public event Action<LogDto>? LogReceived;

        public void NotifyLogReceived(LogDto logDto)
        {
            LogReceived?.Invoke(logDto);
        }
    }
}
