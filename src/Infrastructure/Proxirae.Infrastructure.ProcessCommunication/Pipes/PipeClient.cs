using Proxirae.Application.Pipes;
using System.IO.Pipes;
using System.Runtime.InteropServices;

namespace Proxirae.Infrastructure.ProcessCommunication.Pipes
{
    public sealed class PipeClient : IPipeClient
    {
        private NamedPipeClientStream? _pipe;

        public async Task ConnectAsync(CancellationToken cancellationToken)
        {
            _pipe = new NamedPipeClientStream(
                serverName: ".",
                pipeName: "Proxirae.Network",
                direction: PipeDirection.InOut,
                options: PipeOptions.Asynchronous
            );

            await _pipe.ConnectAsync(cancellationToken);
        }

        public Task DisconnectAsync(CancellationToken cancellationToken)
        {
            if (_pipe != null)
            {
                _pipe.Close();
                _pipe.Dispose(); 
                _pipe = null;    
            }

            return Task.CompletedTask;
        }

        public async Task<ReadOnlyMemory<byte>?> ReadAsync(CancellationToken cancellationToken)
        {
            if (_pipe is null) return null;

            int headerSize = Marshal.SizeOf<Message.PipeMessageHeader>();
            var headerBuffer = new byte[headerSize];

            try
            {
                await _pipe.ReadExactlyAsync(headerBuffer, cancellationToken);

                uint payloadSize = BitConverter.ToUInt32(headerBuffer, 8);

                if (payloadSize == 0)
                {
                    return headerBuffer; 
                }

                var fullMessage = new byte[headerSize + payloadSize];
                Array.Copy(headerBuffer, fullMessage, headerSize);

                await _pipe.ReadExactlyAsync(fullMessage.AsMemory(headerSize, (int)payloadSize), cancellationToken);

                return fullMessage;
            }
            catch (EndOfStreamException)
            {
                return null;
            }
        }

        public async Task WriteAsync(ReadOnlyMemory<byte> message, CancellationToken cancellationToken)
        {
            if (_pipe is null)
            {
                return;
            }

            await _pipe.WriteAsync(message, cancellationToken);
            await _pipe.FlushAsync(cancellationToken);
        }

        public ValueTask DisposeAsync()
        {
            if (_pipe is null)
            {
                return ValueTask.CompletedTask;
            }

            var disposeTask = _pipe.DisposeAsync();
            _pipe = null;

            return disposeTask;
        }
    }
}
