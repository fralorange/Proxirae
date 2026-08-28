namespace Proxirae.Application.Messenger
{
    public interface IMessenger
    {
        Task SendMessageAsync<T>(MessageType msg, T payload, CancellationToken cancellationToken);
        Task SendMessageAsync(MessageType msg, CancellationToken cancellationToken);
    }
}
