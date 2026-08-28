using Proxirae.Contracts.DTOs.Test;

namespace Proxirae.Application.Services.Test
{
    public interface ITestService
    {
        event Action<TestProgressDto>? ProgressReceived;
        void NotifyProgressReceived(TestProgressDto progress);
        Task InitiateTestAsync(TestDto test, CancellationToken cancellationToken);
    }
}
