using Proxirae.Domain.Rules;

namespace Proxirae.Application.UnitsOfWork.Rules
{
    public interface IRuleUnitOfWork
    {
        Task<IReadOnlyCollection<Rule>> GetAsync(CancellationToken token);
        Rule? GetById(Guid id);
        void Add(Rule rule);
        bool Update(Rule rule);
        bool Delete(Guid id);
        Task SaveChangesAsync(CancellationToken token);
    }
}
