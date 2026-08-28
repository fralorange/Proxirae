using Proxirae.Infrastructure.ProcessCommunication.Pipes.Message;

namespace Proxirae.Infrastructure.ProcessCommunication.Channels
{
    public interface ISendChannel
    {
        Task SendAsync(PipeMessage message, CancellationToken cancellationToken);
    }
}
