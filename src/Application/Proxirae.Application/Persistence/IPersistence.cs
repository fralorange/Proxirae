namespace Proxirae.Application.Stores
{
    public interface IPersistence<T> where T : class
    {
        Task SaveAsync(CancellationToken cancellationToken);
    }
}
