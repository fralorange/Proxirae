using Proxirae.Contracts.DTOs.Logs;

namespace Proxirae.Application.Services.Logs
{
    public interface ILogService
    {
        event Action<LogDto>? LogReceived;
        void NotifyLogReceived(LogDto logDto);
    }
}
