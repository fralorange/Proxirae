using Proxirae.Contracts.DTOs.Proxies;
using Proxirae.Domain.Proxies;

namespace Proxirae.Application.Mappers.Proxies
{
    public interface IProxyDtoMapper
    {
        ProxyListDto MapToList(Proxy proxy);
        ProxyDetailDto MapToDetail(Proxy proxy);
        Proxy MapToDomain(ProxyAddDto proxyAddDto);
        Proxy MapToDomain(ProxyEditDto proxyEditDto);
        Proxy MapToDomain(ProxyDetailDto proxyDetailDto);
    }
}
