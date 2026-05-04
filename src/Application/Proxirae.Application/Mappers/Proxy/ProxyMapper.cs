using Proxirae.Contracts.DTOs.Proxy;
using Proxirae.Domain.Proxy;

namespace Proxirae.Application.Mappers.Proxy
{
    public class ProxyMapper : IProxyMapper
    {
        public ProxyListDto MapToList(ProxyServer proxy)
        {
            return new ProxyListDto
            {
                Id = proxy.Id,
                Remarks = proxy.Remarks,
                IP = proxy.IP,
                Port = proxy.Port,
                Type = (ProxyTypeDto)proxy.Type,
            };
        }

        public ProxyDetailDto MapToDetail(ProxyServer proxy)
        {
            return new ProxyDetailDto
            {
                Id = proxy.Id,
                Remarks = proxy.Remarks,
                IP = proxy.IP,
                Port = proxy.Port,
                Type = (ProxyTypeDto)proxy.Type,
                Username = proxy.Username,
                Password = proxy.Password,
            };
        }

        public ProxyServer MapToDomain(ProxyAddDto proxyAddDto)
        {
            return new ProxyServer
            {
                Id = Guid.NewGuid(),
                Remarks = proxyAddDto.Remarks,
                IP = proxyAddDto.IP,
                Port = proxyAddDto.Port,
                Type = (ProxyType)proxyAddDto.Type,
                Username = proxyAddDto.Username,
                Password = proxyAddDto.Password,
            };
        }

        public ProxyServer MapToDomain(ProxyEditDto proxyEditDto)
        {
            return new ProxyServer
            {
                Id = proxyEditDto.Id,
                Remarks = proxyEditDto.Remarks,
                IP = proxyEditDto.IP,
                Port = proxyEditDto.Port,
                Type = (ProxyType)proxyEditDto.Type,
                Username = proxyEditDto.Username,
                Password = proxyEditDto.Password,
            };
        }
    }
}
