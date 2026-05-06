using Proxirae.Domain.Proxies;

namespace Proxirae.Application.Repositories.Proxies
{
    public interface IProxyRepository
    {
        Task<IReadOnlyCollection<Proxy>> GetAsync(CancellationToken token);
        Task<Proxy?> GetByIdAsync(Guid id, CancellationToken token);
        Task AddAsync(Proxy proxyServer, CancellationToken token);
        Task<bool> UpdateAsync(Proxy proxyServer, CancellationToken token);
        Task<bool> DeleteAsync(Guid id, CancellationToken token);
    }
}
