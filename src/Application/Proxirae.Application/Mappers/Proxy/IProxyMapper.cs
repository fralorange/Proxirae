using Proxirae.Contracts.DTOs.Proxy;

namespace Proxirae.Application.Mappers.Proxy
{
    public interface IProxyMapper
    {
        ProxyListDto MapToList(ProxyServer proxy);
        ProxyDetailDto MapToDetail(ProxyServer proxy);
        ProxyServer MapToDomain(ProxyAddDto proxyAddDto);
        ProxyServer MapToDomain(ProxyEditDto proxyEditDto);
    }
}
