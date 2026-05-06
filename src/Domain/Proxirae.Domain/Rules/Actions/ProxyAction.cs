using Proxirae.Domain.Proxies;

namespace Proxirae.Domain.Rules.Actions
{
    public class ProxyAction : BaseAction
    {
        public override Guid Id => Proxy.Id;
        public Proxy Proxy { get; }

        public ProxyAction(Proxy proxy)
        {
            Proxy = proxy;
        }

        public override string Name 
        { 
            get
            {
                if (Proxy.Remarks is not null)
                {
                    return $"Proxy {Proxy.Type} {Proxy.Remarks}";
                } else
                {
                    return $"Proxy {Proxy.Type} {Proxy.IP}:{Proxy.Port}";
                }
            }
        }
    }
}
