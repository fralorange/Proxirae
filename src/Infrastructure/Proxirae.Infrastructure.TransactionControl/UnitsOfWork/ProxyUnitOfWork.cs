using Proxirae.Application.Repositories.Proxy;
using Proxirae.Application.UnitsOfWork.Proxy;
using Proxirae.Domain.Proxy;

namespace Proxirae.Infrastructure.TransactionControl.UnitsOfWork
{
    public class ProxyUnitOfWork : IProxyUnitOfWork
    {
        private readonly IProxyRepository _proxyRepository;
        private readonly List<TrackedEntity<Proxy>> _buffer = [];

        public ProxyUnitOfWork(IProxyRepository proxyRepository)
        {
            _proxyRepository = proxyRepository;
        }

        public async Task<IReadOnlyCollection<Proxy>> GetAsync(CancellationToken token)
        {
            if (_buffer.Count == 0)
            {
                var proxies = await _proxyRepository.GetAsync(token);

                _buffer.AddRange(
                    proxies.Select(p => new TrackedEntity<Proxy>
                    {
                        Entity = p,
                        State = EntityState.Unchanged
                    })
                );
            }

            return _buffer.Select(x => x.Entity).ToList().AsReadOnly();
        }

        public Proxy? GetById(Guid id)
        {
            return _buffer.FirstOrDefault(p => p.Entity.Id == id)?.Entity;
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

            if (item == null)
                return false;

            item.Entity = proxyServer;

            if (item.State == EntityState.Added)
                return true; 

            item.State = EntityState.Modified;

            return true;
        }

        public bool Delete(Guid id)
        {
            var item = _buffer.FirstOrDefault(x => x.Entity.Id == id);

            if (item == null)
                return false;

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
                        item.State = EntityState.Unchanged;
                        break;

                    case EntityState.Modified:
                        await _proxyRepository.UpdateAsync(item.Entity, token);
                        item.State = EntityState.Unchanged;
                        break;

                    case EntityState.Deleted:
                        await _proxyRepository.DeleteAsync(item.Entity.Id, token);
                        item.State = EntityState.Unchanged;
                        break;
                }
            }
            
            _buffer.RemoveAll(x => x.State == EntityState.Deleted);

            foreach (var item in _buffer)
                item.State = EntityState.Unchanged;
        }
    }
}
