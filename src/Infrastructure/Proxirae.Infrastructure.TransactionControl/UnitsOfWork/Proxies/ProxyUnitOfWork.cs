using Proxirae.Application.Repositories;
using Proxirae.Application.Stores;
using Proxirae.Application.UnitsOfWork.Proxies;
using Proxirae.Domain.Proxies;

namespace Proxirae.Infrastructure.TransactionControl.UnitsOfWork.Proxies
{
    public class ProxyUnitOfWork : IProxyUnitOfWork
    {
        private readonly ICrudRepository<Proxy> _proxyRepository;
        private readonly IPersistence<Proxy> _proxyPersistence;

        private readonly List<TrackedEntity<Proxy>> _buffer = [];
        private bool _isLoadedAll = false;

        public ProxyUnitOfWork(ICrudRepository<Proxy> proxyRepository, IPersistence<Proxy> proxyPersistence)
        {
            _proxyRepository = proxyRepository;
            _proxyPersistence = proxyPersistence;
        }

        public async Task<IReadOnlyCollection<Proxy>> GetAsync(CancellationToken token)
        {
            if (!_isLoadedAll)
            {
                var proxies = await _proxyRepository.GetAsync(token);

                foreach (var proxy in proxies)
                {
                    var tracked = _buffer.FirstOrDefault(x => x.Entity.Id == proxy.Id);

                    if (tracked is null)
                    {
                        _buffer.Add(new TrackedEntity<Proxy>
                        {
                            Entity = proxy,
                            State = EntityState.Unchanged
                        });
                    }
                }

                _isLoadedAll = true;
            }

            return _buffer
                .Where(x => x.State != EntityState.Deleted)
                .Select(te => te.Entity)
                .ToList();
        }

        public async Task<Proxy?> GetByIdAsync(Guid id, CancellationToken token)
        {
            var tracked = _buffer.FirstOrDefault(p => p.Entity.Id == id);

            if (tracked is not null)
            {
                return tracked.State == EntityState.Deleted ? null : tracked.Entity;
            }

            var proxy = await _proxyRepository.GetByIdAsync(id, token);
            if (proxy is null) return null;

            _buffer.Add(new TrackedEntity<Proxy>
            {
                Entity = proxy,
                State = EntityState.Unchanged
            });

            return proxy;
        }

        public void Add(Proxy proxyServer)
        {
            _buffer.Add(new TrackedEntity<Proxy>
            {
                Entity = proxyServer,
                State = EntityState.Added
            });
        }

        public bool Update(Proxy proxyServer)
        {
            var item = _buffer.FirstOrDefault(x => x.Entity.Id == proxyServer.Id);

            if (item is null)
            {
                _buffer.Add(new TrackedEntity<Proxy>
                {
                    Entity = proxyServer,
                    State = EntityState.Modified
                });
                return true;
            }

            if (item.State == EntityState.Deleted)
                return false;

            item.Entity = proxyServer;
            if (item.State != EntityState.Added)
                item.State = EntityState.Modified;

            return true;
        }

        public bool Delete(Guid id)
        {
            var item = _buffer.FirstOrDefault(x => x.Entity.Id == id);

            if (item is null)
            {
                var stubEntity = new Proxy { Id = id };
                _buffer.Add(new TrackedEntity<Proxy>
                {
                    Entity = stubEntity,
                    State = EntityState.Deleted
                });
                return true;
            }

            if (item.State == EntityState.Added)
            {
                _buffer.Remove(item);
                return true;
            }

            item.State = EntityState.Deleted;
            return true;
        }

        public async Task SaveChangesAsync(CancellationToken token)
        {
            foreach (var item in _buffer)
            {
                switch (item.State)
                {
                    case EntityState.Added:
                        await _proxyRepository.AddAsync(item.Entity, token);
                        break;
                    case EntityState.Modified:
                        await _proxyRepository.UpdateAsync(item.Entity, token);
                        break;
                    case EntityState.Deleted:
                        await _proxyRepository.DeleteAsync(item.Entity.Id, token);
                        break;
                }
            }

            await _proxyPersistence.SaveAsync(token);

            _buffer.RemoveAll(x => x.State == EntityState.Deleted);

            foreach (var item in _buffer)
                item.State = EntityState.Unchanged;
        }
    }
}