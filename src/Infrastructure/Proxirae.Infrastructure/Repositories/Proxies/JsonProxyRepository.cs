using Proxirae.Application.Repositories;
using Proxirae.Domain.Proxies;
using Proxirae.Infrastructure.Persistence.JSON;

namespace Proxirae.Infrastructure.Repositories.Proxies
{
    public class JsonProxyRepository : JsonPersistence<Proxy>, ICrudRepository<Proxy>
    {
        public JsonProxyRepository(string filePath) : base(filePath) { }

        public async Task<IReadOnlyCollection<Proxy>> GetAsync(CancellationToken token)
        {
            await EnsureLoadedAsync(token);

            return _items.AsReadOnly();
        }

        public async Task<Proxy?> GetByIdAsync(Guid id, CancellationToken token)
        {
            await EnsureLoadedAsync(token);

            return _items.FirstOrDefault(p => p.Id == id);
        }

        public async Task AddAsync(Proxy proxyServer, CancellationToken token)
        {
            await EnsureLoadedAsync(token);

            _items.Add(proxyServer);
        }

        public async Task<bool> UpdateAsync(Proxy proxyServer, CancellationToken token)
        {
            await EnsureLoadedAsync(token);

            var index = _items.FindIndex(p => p.Id == proxyServer.Id);

            if (index == -1)
                return false;

            _items[index] = proxyServer;

            return true;
        }

        public async Task<bool> DeleteAsync(Guid id, CancellationToken token)
        {
            await EnsureLoadedAsync(token);

            return _items.RemoveAll(p => p.Id == id) > 0;
        }
    }
}
