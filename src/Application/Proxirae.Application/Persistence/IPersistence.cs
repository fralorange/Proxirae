namespace Proxirae.Application.Persistence
{
    public interface IPersistence<T> where T : class
    {
        Task SaveAsync(CancellationToken cancellationToken);
        Task ReloadAsync(CancellationToken cancellationToken);
    }
}
