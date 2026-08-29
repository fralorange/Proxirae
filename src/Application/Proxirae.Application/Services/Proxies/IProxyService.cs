using Proxirae.Contracts.DTOs.Proxies;

namespace Proxirae.Application.Services.Proxies
{
    public interface IProxyService
    {
        Task<IReadOnlyCollection<ProxyListDto>> GetAsync(CancellationToken token);
        Task<ProxyDetailDto?> GetByIdAsync(Guid id, CancellationToken token);
        Task<ProxyListDto> AddAsync(ProxyAddDto proxyAddDto, CancellationToken token);
        Task<ProxyListDto?> UpdateAsync(ProxyEditDto proxyEditDto, CancellationToken token);
        Task<bool> DeleteAsync(Guid id, CancellationToken token);
        Task SaveChangesAsync(CancellationToken token);
    }
}
