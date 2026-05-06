using Proxirae.Application.Repositories.Proxies;
using Proxirae.Domain.Proxies;

namespace Proxirae.Infrastructure.Repositories.Proxies
{
    public class InMemoryProxyRepository : IProxyRepository
    {
        private readonly List<Proxy> _proxyServers = [];

        public Task<IReadOnlyCollection<Proxy>> GetAsync(CancellationToken token)
        {
            return Task.FromResult<IReadOnlyCollection<Proxy>>(_proxyServers.AsReadOnly());
        }

        public Task<Proxy?> GetByIdAsync(Guid id, CancellationToken token)
        {
            return Task.FromResult(_proxyServers.FirstOrDefault(p => p.Id == id));
        }

        public Task AddAsync(Proxy proxyServer, CancellationToken token)
        {
            _proxyServers.Add(proxyServer);
            return Task.CompletedTask;
        }

        public Task<bool> UpdateAsync(Proxy proxyServer, CancellationToken token)
        {
            var index = _proxyServers.FindIndex(p => p.Id == proxyServer.Id);
            if (index == -1) return Task.FromResult(false);
            _proxyServers[index] = proxyServer;
            return Task.FromResult(true);
        }

        public Task<bool> DeleteAsync(Guid id, CancellationToken token)
        {
            return Task.FromResult(_proxyServers.RemoveAll(p => p.Id == id) > 0);
        }
    }
}
