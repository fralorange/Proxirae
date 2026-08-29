namespace Proxirae.Application.Repositories
{
    public interface ICrudRepository<T> where T : class
    {
        Task<IReadOnlyCollection<T>> GetAsync(CancellationToken token);
        Task<T?> GetByIdAsync(Guid id, CancellationToken token);
        Task AddAsync(T item, CancellationToken token);
        Task<bool> UpdateAsync(T item, CancellationToken token);
        Task<bool> DeleteAsync(Guid id, CancellationToken token);
    }
}
