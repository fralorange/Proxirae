namespace Proxirae.Domain.Rules
{
    [Flags]
    public enum RuleProtocol
    {
        None = 0,
        TCP = 1,
        UDP = 2,
    }
}
