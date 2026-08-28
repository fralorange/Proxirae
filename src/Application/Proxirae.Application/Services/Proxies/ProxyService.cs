using Proxirae.Application.Mappers.Proxies;
using Proxirae.Application.Messenger;
using Proxirae.Application.UnitsOfWork.Proxies;
using Proxirae.Contracts.DTOs.Proxies;

namespace Proxirae.Application.Services.Proxies
{
    public class ProxyService : IProxyService
    {
        private readonly IProxyUnitOfWork _proxyUnitOfWork;
        private readonly IProxyDtoMapper _proxyMapper;
        private readonly IMessenger _messenger;

        public ProxyService(IProxyUnitOfWork proxyUnitOfWork, IProxyDtoMapper proxyMapper, IMessenger messenger)
        {
            _proxyUnitOfWork = proxyUnitOfWork;
            _proxyMapper = proxyMapper;
            _messenger = messenger;
        }

        public async Task<IReadOnlyCollection<ProxyListDto>> GetAsync(CancellationToken token)
        {
            var proxies = await _proxyUnitOfWork.GetAsync(token);
            return proxies.Select(_proxyMapper.MapToList).ToList();
        }

        public async Task<ProxyDetailDto?> GetByIdAsync(Guid id, CancellationToken token)
        {
            var proxy = await _proxyUnitOfWork.GetByIdAsync(id, token);
            return proxy is null ? null : _proxyMapper.MapToDetail(proxy);
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

        public async Task SaveChangesAsync(CancellationToken token)
        {
            await _proxyUnitOfWork.SaveChangesAsync(token);

            await _messenger.SendMessageAsync(MessageType.Cmd_ReloadProxies, token);
        }
    }
}
