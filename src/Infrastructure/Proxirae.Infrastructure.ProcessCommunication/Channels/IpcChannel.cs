using Proxirae.Application.Pipes;
using Proxirae.Infrastructure.ProcessCommunication.Pipes.Message;
using Proxirae.Infrastructure.ProcessCommunication.Pipes.Message.Handlers;
using System.Runtime.InteropServices;

namespace Proxirae.Infrastructure.ProcessCommunication.Channels
{
    public class IpcChannel : IConnectChannel, ISendChannel, IReceiveChannel, IDispatchChannel
    {
        private readonly IPipeClient _pipe;
        private readonly Dictionary<ushort, IPipeMessageHandler> _handlers = [];
        private readonly object _handlersLock = new();

        public IpcChannel(IPipeClient pipe)
        {
            _pipe = pipe;
        }

        public Task ConnectAsync(CancellationToken cancellationToken)
        {
            return _pipe.ConnectAsync(cancellationToken);
        }

        public Task DisconnectAsync(CancellationToken cancellationToken)
        {
            return _pipe.DisconnectAsync(cancellationToken);
        }

        public async Task SendAsync(PipeMessage message, CancellationToken cancellationToken)
        {
            var header = message.Header;
            header.Magic = 0x5850;
            header.PayloadSize = (uint)message.Payload.Length;

            var headerSize = Marshal.SizeOf<PipeMessageHeader>();
            var buffer = new byte[headerSize + header.PayloadSize];

            MemoryMarshal.Write(buffer.AsSpan(0, headerSize), in header);

            if (header.PayloadSize > 0)
            {
                message.Payload.AsSpan().CopyTo(buffer.AsSpan(headerSize));
            }

            await _pipe.WriteAsync(buffer, cancellationToken);
        }

        public async Task<PipeMessage?> ReceiveAsync(CancellationToken cancellationToken)
        {
            var rawData = await _pipe.ReadAsync(cancellationToken);

            if (rawData is null)
            {
                return null;
            }

            if (PipeMessage.TryParse(rawData.Value.Span, out var message))
            {
                return message;
            }

            return null;
        }

        public async Task DispatchAsync(PipeMessage message, CancellationToken cancellationToken)
        {
            IPipeMessageHandler? handler;

            lock (_handlersLock)
            {
                _handlers.TryGetValue(message.Header.Type, out handler);
            }

            if (handler != null)
            {
                await handler.HandleAsync(message, cancellationToken);
            }
        }

        public void RegisterHandler(ushort messageType, IPipeMessageHandler handler)
        {
            lock (_handlersLock)
            {
                _handlers.Add(messageType, handler);
            }
        }
    }
}
