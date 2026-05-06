namespace Proxirae.Contracts.DTOs.Rules
{
    [Flags]
    public enum RuleProtocolDto
    {
        None = 0,
        TCP = 1,
        UDP = 2,
    }
}
