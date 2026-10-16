namespace Proxirae.Infrastructure.ProcessCommunication.Pipes.Message.Handlers
{
    public abstract class SafePipeMessageHandler : IPipeMessageHandler
    {
        public async Task HandleAsync(
            PipeMessage message,
            CancellationToken cancellationToken)
        {
            try
            {
                await HandleUnsafeAsync(message, cancellationToken)
                    .ConfigureAwait(false);
            }
            catch (OperationCanceledException)
                when (cancellationToken.IsCancellationRequested)
            {
                throw;
            }
            catch (Exception)
            {
            }
        }

        protected abstract Task HandleUnsafeAsync(
            PipeMessage message,
            CancellationToken cancellationToken);
    }
}