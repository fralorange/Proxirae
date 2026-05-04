namespace Proxirae.Application.UnitsOfWork.Proxy
{
    public interface IProxyUnitOfWork
    {
        Task<IReadOnlyCollection<ProxyServer>> GetAsync(CancellationToken token);
        ProxyServer? GetById(Guid id);
        void Add(ProxyServer proxyServer);
        bool Update(ProxyServer proxyServer);
        bool Delete(Guid id);
        Task SaveChangesAsync(CancellationToken token);
    }
}
