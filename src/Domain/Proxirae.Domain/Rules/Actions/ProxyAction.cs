using Proxirae.Domain.Proxies;

namespace Proxirae.Domain.Rules.Actions
{
    public class ProxyAction : BaseAction
    {
        public override Guid Id { get; }
        public override string Name { get; }

        public ProxyAction(string name, Guid proxyId)
        {
            Id = proxyId;
            Name = name;
        }

        public static ProxyAction CreateFrom(Proxy proxy)
        {
            var name = proxy.Remarks is not null
                ? $"Proxy {proxy.Type} {proxy.Remarks}"
                : $"Proxy {proxy.Type} {proxy.Address}:{proxy.Port}";

            return new ProxyAction(name, proxy.Id);
        }
    }
}
