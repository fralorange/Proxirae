using Proxirae.Application.Repositories;
using Proxirae.Contracts.Data.Rules;
using Proxirae.Infrastructure.Persistence.JSON;

namespace Proxirae.Infrastructure.Repositories.Rules
{
    public class JsonRuleRepository : JsonPersistence<RuleData>, ICrudRepository<RuleData>
    {
        public JsonRuleRepository(string filePath) : base(filePath) { }

        public async Task<IReadOnlyCollection<RuleData>> GetAsync(CancellationToken token)
        {
            await EnsureLoadedAsync(token);

            return _items.AsReadOnly();
        }

        public async Task<RuleData?> GetByIdAsync(Guid id, CancellationToken token)
        {
            await EnsureLoadedAsync(token);

            return _items.FirstOrDefault(p => p.Id == id);
        }

        public async Task AddAsync(RuleData rule, CancellationToken token)
        {
            await EnsureLoadedAsync(token);

            _items.Add(rule);
        }

        public async Task<bool> UpdateAsync(RuleData rule, CancellationToken token)
        {
            await EnsureLoadedAsync(token);

            var index = _items.FindIndex(p => p.Id == rule.Id);

            if (index == -1)
                return false;

            _items[index] = rule;

            return true;
        }

        public async Task<bool> DeleteAsync(Guid id, CancellationToken token)
        {
            await EnsureLoadedAsync(token);

            return _items.RemoveAll(p => p.Id == id) > 0;
        }
    }
}
