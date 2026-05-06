using Proxirae.Contracts.DTOs.Proxies;
using Proxirae.Domain.Proxies;

namespace Proxirae.Application.Mappers.Proxies
{
    public class ProxyMapper : IProxyMapper
    {
        public ProxyListDto MapToList(Proxy proxy)
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

        public ProxyDetailDto MapToDetail(Proxy proxy)
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

        public Proxy MapToDomain(ProxyAddDto proxyAddDto)
        {
            return new Proxy
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

        public Proxy MapToDomain(ProxyEditDto proxyEditDto)
        {
            return new Proxy
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

        public Proxy MapToDomain(ProxyDetailDto proxyDetailDto)
        {
            return new Proxy
            {
                Id = proxyDetailDto.Id,
                Remarks = proxyDetailDto.Remarks,
                IP = proxyDetailDto.IP,
                Port = proxyDetailDto.Port,
                Type = (ProxyType)proxyDetailDto.Type,
                Username = proxyDetailDto.Username,
                Password = proxyDetailDto.Password,
            };
        }
    }
}
