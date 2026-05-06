using Proxirae.Contracts.DTOs.Rules;

namespace Proxirae.Application.Services.Rules
{
    public interface IRuleService
    {
        Task<IReadOnlyCollection<RuleDto>> GetAsync(CancellationToken token);
        Task<RuleDto?> GetByIdAsync(Guid id, CancellationToken token);
        Task<RuleDto> AddAsync(RuleAddDto ruleAddDto, CancellationToken token);
        Task<RuleDto?> UpdateAsync(RuleEditDto ruleEditDto, CancellationToken token);
        Task<bool> DeleteAsync(Guid id, CancellationToken token);
        Task SaveChangesAsync(CancellationToken token);
    }
}
