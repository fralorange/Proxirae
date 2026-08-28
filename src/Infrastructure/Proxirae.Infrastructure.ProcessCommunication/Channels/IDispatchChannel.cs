using Proxirae.Infrastructure.ProcessCommunication.Pipes.Message;
using Proxirae.Infrastructure.ProcessCommunication.Pipes.Message.Handlers;

namespace Proxirae.Infrastructure.ProcessCommunication.Channels
{
    public interface IDispatchChannel
    {
        Task DispatchAsync(PipeMessage message, CancellationToken cancellationToken);
        void RegisterHandler(ushort messageType, IPipeMessageHandler handler);
    }
}
