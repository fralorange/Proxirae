using Proxirae.Infrastructure.ProcessCommunication.Pipes.Message;

namespace Proxirae.Infrastructure.ProcessCommunication.Channels
{
    public interface IReceiveChannel
    {
        Task<PipeMessage?> ReceiveAsync(CancellationToken cancellationToken);
    }
}
