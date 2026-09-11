using Proxirae.Presentation.WPF.Models.Process;

namespace Proxirae.Presentation.WPF.Services.Process
{
    public interface IProcessInfoService
    {
        Task<ProcessInfo?> GetProcessInfoAsync(long processId, CancellationToken cancellationToken);
    }
}
