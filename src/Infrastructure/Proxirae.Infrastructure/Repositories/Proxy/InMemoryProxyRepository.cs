using Proxirae.Application.Repositories.Proxy;

namespace Proxirae.Infrastructure.Repositories.Proxy
{
    public class InMemoryProxyRepository : IProxyRepository
    {
        private readonly List<ProxyServer> _proxyServers = [];

        public Task<IReadOnlyCollection<ProxyServer>> GetAsync(CancellationToken token)
        {
            return Task.FromResult<IReadOnlyCollection<ProxyServer>>(_proxyServers.AsReadOnly());
        }

        public Task<ProxyServer?> GetByIdAsync(Guid id, CancellationToken token)
        {
            return Task.FromResult(_proxyServers.FirstOrDefault(p => p.Id == id));
        }

        public Task AddAsync(ProxyServer proxyServer, CancellationToken token)
        {
            _proxyServers.Add(proxyServer);
            return Task.CompletedTask;
        }

        public Task<bool> UpdateAsync(ProxyServer proxyServer, CancellationToken token)
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
