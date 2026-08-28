using Proxirae.Domain.Rules;

namespace Proxirae.Application.UnitsOfWork.Rules
{
    public interface IRuleUnitOfWork
    {
        Task<IReadOnlyCollection<Rule>> GetAsync(CancellationToken token);
        Task<Rule?> GetByIdAsync(Guid id, CancellationToken token);
        void Add(Rule rule);
        bool Update(Rule rule);
        bool Delete(Guid id);
        Task SaveChangesAsync(CancellationToken token);
    }
}
