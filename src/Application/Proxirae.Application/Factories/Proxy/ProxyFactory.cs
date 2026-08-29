using Proxirae.Contracts.DTOs.Proxies;

namespace Proxirae.Application.Factories.Proxy
{
    public static class ProxyFactory
    {
        public static ProxyAddDto CreateAddDto(
            string address,
            int port,
            ProxyTypeDto type,
            string? username,
            string? password,
            string? remarks)
        {
            return new ProxyAddDto
            {
                Address = address,
                Port = port,
                Type = type,
                Username = username,
                Password = password,
                Remarks = remarks
            };
        }

        public static ProxyEditDto CreateEditDto(
            Guid id,
            string address,
            int port,
            ProxyTypeDto type,
            string? username,
            string? password,
            string? remarks)
        {
            return new ProxyEditDto
            {
                Id = id,
                Address = address,
                Port = port,
                Type = type,
                Username = username,
                Password = password,
                Remarks = remarks
            };
        }

        public static ProxyTestDto CreateTestDto(
            string address,
            int port,
            ProxyTypeDto type,
            string? username,
            string? password)
        {
            return new ProxyTestDto
            {
                Address = address,
                Port = port,
                Type = type,
                Username = username,
                Password = password
            };
        }
    }
}
