using Proxirae.Contracts.DTOs.Proxies;

namespace Proxirae.Contracts.DTOs.Rules.Actions
{
    public class ProxyActionDto : BaseActionDto
    {
        public override string Name { get; }
        public ProxyDetailDto Proxy { get; set; }

        public ProxyActionDto(string name, ProxyDetailDto proxy)
        {
            Id = proxy.Id;
            Name = name;
            Proxy = proxy;
        }
    }
}
