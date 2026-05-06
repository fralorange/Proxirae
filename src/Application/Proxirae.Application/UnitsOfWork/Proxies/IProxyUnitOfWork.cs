using Proxirae.Domain.Proxies;

namespace Proxirae.Application.UnitsOfWork.Proxies
{
    public interface IProxyUnitOfWork
    {
        Task<IReadOnlyCollection<Proxy>> GetAsync(CancellationToken token);
        Proxy? GetById(Guid id);
        void Add(Proxy proxyServer);
        bool Update(Proxy proxyServer);
        bool Delete(Guid id);
        Task SaveChangesAsync(CancellationToken token);
    }
}
