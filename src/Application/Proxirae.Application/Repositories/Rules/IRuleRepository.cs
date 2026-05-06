using Proxirae.Domain.Rules;

namespace Proxirae.Application.Repositories.Rules
{
    public interface IRuleRepository
    {
        Task<IReadOnlyCollection<Rule>> GetAsync(CancellationToken token);
        Task<Rule?> GetByIdAsync(Guid id, CancellationToken token);
        Task AddAsync(Rule rule, CancellationToken token);
        Task<bool> UpdateAsync(Rule rule, CancellationToken token);
        Task<bool> DeleteAsync(Guid id, CancellationToken token);
    }
}
