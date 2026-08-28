namespace Proxirae.Application.Pipes
{
    public interface IPipeClient : IAsyncDisposable
    {
        Task ConnectAsync(CancellationToken cancellationToken);
        Task DisconnectAsync(CancellationToken cancellationToken);
        Task WriteAsync(ReadOnlyMemory<byte> message, CancellationToken cancellationToken);
        Task<ReadOnlyMemory<byte>?> ReadAsync(CancellationToken cancellationToken);
    }
}
