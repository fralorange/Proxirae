using Proxirae.Application.Messenger;
using Proxirae.Contracts.DTOs.Test;

namespace Proxirae.Application.Services.Test
{
    public class TestService : ITestService
    {
        private readonly IMessenger _messenger;

        public event Action<TestProgressDto>? ProgressReceived;

        public TestService(IMessenger messenger)
        {
            _messenger = messenger;
        }

        public void NotifyProgressReceived(TestProgressDto progress)
        {
            ProgressReceived?.Invoke(progress);
        }

        public async Task InitiateTestAsync(TestDto test, CancellationToken cancellationToken)
        {
            await _messenger.SendMessageAsync(MessageType.Cmd_CheckProxy, test, cancellationToken);
        }
    }
}
