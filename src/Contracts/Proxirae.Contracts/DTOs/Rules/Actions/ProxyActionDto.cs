namespace Proxirae.Contracts.DTOs.Rules.Actions
{
    public class ProxyActionDto : BaseActionDto
    {
        public override string Name { get; }

        public ProxyActionDto(string name, Guid proxyId)
        {
            Id = proxyId;
            Name = name;
        }
    }
}
