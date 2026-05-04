using Proxirae.Application.Mappers.Proxy;
using Proxirae.Application.UnitsOfWork.Proxy;
using Proxirae.Contracts.DTOs.Proxy;

namespace Proxirae.Application.Services.Proxy
{
    public class ProxyService : IProxyService
    {
        private readonly IProxyUnitOfWork _proxyUnitOfWork;
        private readonly IProxyMapper _proxyMapper;

        public ProxyService(IProxyUnitOfWork proxyUnitOfWork, IProxyMapper proxyMapper)
        {
            _proxyUnitOfWork = proxyUnitOfWork;
            _proxyMapper = proxyMapper;
        }

        public async Task<IReadOnlyCollection<ProxyListDto>> GetAsync(CancellationToken token)
        {
            var proxies = await _proxyUnitOfWork.GetAsync(token);
            return proxies.Select(_proxyMapper.MapToList).ToList();
        }

        public Task<ProxyDetailDto?> GetByIdAsync(Guid id, CancellationToken token)
        {
            var proxy = _proxyUnitOfWork.GetById(id);
            return Task.FromResult(proxy is null ? null : _proxyMapper.MapToDetail(proxy));
        }

        public Task<ProxyListDto> AddAsync(ProxyAddDto proxyAddDto, CancellationToken token)
        {
            var proxy = _proxyMapper.MapToDomain(proxyAddDto);
            _proxyUnitOfWork.Add(proxy);
            return Task.FromResult(_proxyMapper.MapToList(proxy));
        }

        public Task<ProxyListDto?> UpdateAsync(ProxyEditDto proxyEditDto, CancellationToken token)
        {
            var proxy = _proxyMapper.MapToDomain(proxyEditDto);
            var result = _proxyUnitOfWork.Update(proxy);
            if (result)
            {
                return Task.FromResult<ProxyListDto?>(_proxyMapper.MapToList(proxy));
            }
            else
            {
                return Task.FromResult<ProxyListDto?>(null);
            } 
        }

        public Task<bool> DeleteAsync(Guid id, CancellationToken token)
        {
            return Task.FromResult(_proxyUnitOfWork.Delete(id));
        }

        public Task SaveChangesAsync(CancellationToken token)
        {
            return _proxyUnitOfWork.SaveChangesAsync(token);
        }
    }
}
