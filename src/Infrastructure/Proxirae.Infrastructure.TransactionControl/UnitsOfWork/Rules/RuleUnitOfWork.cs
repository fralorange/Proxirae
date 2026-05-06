using Proxirae.Application.Repositories.Rules;
using Proxirae.Application.UnitsOfWork.Rules;
using Proxirae.Domain.Rules;

namespace Proxirae.Infrastructure.TransactionControl.UnitsOfWork.Rules
{
    public class RuleUnitOfWork : IRuleUnitOfWork
    {
        private readonly IRuleRepository _ruleRepository;
        private readonly List<TrackedEntity<Rule>> _buffer = [];

        public RuleUnitOfWork(IRuleRepository ruleRepository)
        {
            _ruleRepository = ruleRepository;
        }

        public async Task<IReadOnlyCollection<Rule>> GetAsync(CancellationToken token)
        {
            if (_buffer.Count == 0)
            {
                var rules = await _ruleRepository.GetAsync(token);
                _buffer.AddRange(
                    rules.Select(r => new TrackedEntity<Rule>
                    {
                        Entity = r,
                        State = EntityState.Unchanged
                    })
                );
            }

            return _buffer.Select(te => te.Entity).ToList();
        }

        public Rule? GetById(Guid id)
        {
            return _buffer.FirstOrDefault(r => r.Entity.Id == id)?.Entity;
        }

        public void Add(Rule rule)
        {
            _buffer.Add(new TrackedEntity<Rule>
            {
                Entity = rule,
                State = EntityState.Added
            });
        }

        public bool Update(Rule rule)
        {
            var item = _buffer.FirstOrDefault(x => x.Entity.Id == rule.Id);

            if (item is null)
                return false;

            item.Entity = rule;

            if (item.State == EntityState.Added)
                return true;

            item.State = EntityState.Modified;

            return true;
        }

        public bool Delete(Guid id)
        {
            var item = _buffer.FirstOrDefault(x => x.Entity.Id == id);

            if (item is null)
                return false;

            if (item.State == EntityState.Added)
            {
                _buffer.Remove(item);
                return true;
            }

            item.State = EntityState.Deleted;

            return true;
        }

        public async Task SaveChangesAsync(CancellationToken token)
        {
            foreach (var item in _buffer)
            {
                switch (item.State)
                {
                    case EntityState.Added:
                        await _ruleRepository.AddAsync(item.Entity, token);
                        break;
                    case EntityState.Modified:
                        await _ruleRepository.UpdateAsync(item.Entity, token);
                        break;
                    case EntityState.Deleted:
                        await _ruleRepository.DeleteAsync(item.Entity.Id, token);
                        break;
                }
            }

            _buffer.RemoveAll(x => x.State == EntityState.Deleted);

            foreach (var item in _buffer)
                item.State = EntityState.Unchanged;
        }
    }
}
