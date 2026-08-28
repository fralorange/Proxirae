namespace Proxirae.Infrastructure.ProcessCommunication.Pipes.Message.Handlers
{
    public interface IPipeMessageHandler
    {
        Task HandleAsync(PipeMessage message, CancellationToken cancellationToken);
    }
}
