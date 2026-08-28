using Proxirae.Contracts.DTOs.Proxies;

namespace Proxirae.Contracts.DTOs.Test
{
    public class TestDto
    {
        public Guid Id { get; set; }
        public required ProxyTestDto Proxy { get; set; }
        public required string TestAddress { get; set; }
        public ushort TestPort { get; set; }
    }
}
