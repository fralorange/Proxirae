using Proxirae.Contracts.DTOs.Proxies;
using Proxirae.Domain.Proxies;

namespace Proxirae.Application.Mappers.Proxies
{
    public class ProxyDtoMapper : IProxyDtoMapper
    {
        public ProxyListDto MapToList(Proxy proxy)
        {
            return new ProxyListDto
            {
                Id = proxy.Id,
                Remarks = proxy.Remarks,
                Address = proxy.Address,
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
                Address = proxy.Address,
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
                Address = proxyAddDto.Address,
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
                Address = proxyEditDto.Address,
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
                Address = proxyDetailDto.Address,
                Port = proxyDetailDto.Port,
                Type = (ProxyType)proxyDetailDto.Type,
                Username = proxyDetailDto.Username,
                Password = proxyDetailDto.Password,
            };
        }
    }
}
