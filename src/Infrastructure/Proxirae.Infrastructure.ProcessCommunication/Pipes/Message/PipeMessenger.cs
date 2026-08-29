using Proxirae.Application.Messenger;
using Proxirae.Infrastructure.ProcessCommunication.Channels;
using System.Text.Json;

namespace Proxirae.Infrastructure.ProcessCommunication.Pipes.Message
{
    public class PipeMessenger : IMessenger
    {
        private readonly ISendChannel _sender;

        public PipeMessenger(ISendChannel sender)
        {
            _sender = sender;
        }

        public Task SendMessageAsync<T>(MessageType msg, T payload, CancellationToken cancellationToken)
        {
            var rawType = (ushort)msg;
            var rawPayload = JsonSerializer.SerializeToUtf8Bytes(payload);

            var pipeMessage = PipeMessage.Create(rawType, rawPayload);

            return _sender.SendAsync(pipeMessage, cancellationToken);
        }

        public Task SendMessageAsync(MessageType msg, CancellationToken cancellationToken)
        {
            var rawType = (ushort)msg;

            var pipeSignal = PipeMessage.Create(rawType);

            return _sender.SendAsync(pipeSignal, cancellationToken);
        }
    }
}
