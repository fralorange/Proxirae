namespace Proxirae.Application.Repositories.Proxy
{
    public interface IProxyRepository
    {
        Task<IReadOnlyCollection<ProxyServer>> GetAsync(CancellationToken token);
        Task<ProxyServer?> GetByIdAsync(Guid id, CancellationToken token);
        Task AddAsync(ProxyServer proxyServer, CancellationToken token);
        Task<bool> UpdateAsync(ProxyServer proxyServer, CancellationToken token);
        Task<bool> DeleteAsync(Guid id, CancellationToken token);
    }
}
