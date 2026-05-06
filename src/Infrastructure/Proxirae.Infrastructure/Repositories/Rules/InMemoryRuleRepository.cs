using Proxirae.Application.Repositories.Rules;
using Proxirae.Domain.Rules;

namespace Proxirae.Infrastructure.Repositories.Rules
{
    public class InMemoryRuleRepository : IRuleRepository
    {
        private readonly List<Rule> _rules = [];

        public Task<IReadOnlyCollection<Rule>> GetAsync(CancellationToken token)
        {
            return Task.FromResult<IReadOnlyCollection<Rule>>(_rules);
        }

        public Task<Rule?> GetByIdAsync(Guid id, CancellationToken token)
        {
            return Task.FromResult(_rules.FirstOrDefault(r => r.Id == id));
        }

        public Task AddAsync(Rule rule, CancellationToken token)
        {
            _rules.Add(rule);
            return Task.CompletedTask;
        }

        public Task<bool> UpdateAsync(Rule rule, CancellationToken token)
        {
            var index = _rules.FindIndex(r => r.Id == rule.Id);
            if (index == -1) return Task.FromResult(false);
            _rules[index] = rule;
            return Task.FromResult(true);
        }

        public Task<bool> DeleteAsync(Guid id, CancellationToken token)
        {
            return Task.FromResult(_rules.RemoveAll(r => r.Id == id) > 0);
        }
    }
}
