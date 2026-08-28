namespace Proxirae.Infrastructure.ProcessCommunication.Channels
{
    public interface IConnectChannel
    {
        Task ConnectAsync(CancellationToken cancellationToken);
        Task DisconnectAsync(CancellationToken cancellationToken);
    }
}
