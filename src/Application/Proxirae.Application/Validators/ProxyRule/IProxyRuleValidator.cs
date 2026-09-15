using Proxirae.Contracts.DTOs.Rules;

namespace Proxirae.Application.Validators.ProxyRule
{
    public interface IProxyRuleValidator
    {
        Task<IReadOnlyCollection<RuleDto>> GetAffectedRulesAsync(Guid proxyId, CancellationToken cancellationToken);
        Task UnbindProxyFromRulesAsync(Guid proxyId, CancellationToken cancellationToken);
        Task SaveChangesAsync(CancellationToken token);
    }
}
