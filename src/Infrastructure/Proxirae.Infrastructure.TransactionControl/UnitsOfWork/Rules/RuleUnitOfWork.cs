using Proxirae.Application.Mappers.Rules;
using Proxirae.Application.Repositories;
using Proxirae.Application.Stores;
using Proxirae.Application.UnitsOfWork.Rules;
using Proxirae.Contracts.Data.Rules;
using Proxirae.Domain.Rules;

namespace Proxirae.Infrastructure.TransactionControl.UnitsOfWork.Rules
{
    public class RuleUnitOfWork : IRuleUnitOfWork
    {
        private readonly ICrudRepository<RuleData> _ruleRepository;
        private readonly IPersistence<RuleData> _rulePersistence;
        private readonly IRuleDataMapper _ruleDataMapper;

        private readonly List<TrackedEntity<Rule>> _buffer = [];
        private bool _isLoadedAll = false;

        public RuleUnitOfWork(ICrudRepository<RuleData> ruleRepository, IPersistence<RuleData> rulePersistence, IRuleDataMapper ruleDataMapper)
        {
            _ruleRepository = ruleRepository;
            _rulePersistence = rulePersistence;
            _ruleDataMapper = ruleDataMapper;
        }

        public async Task<IReadOnlyCollection<Rule>> GetAsync(CancellationToken token)
        {
            if (!_isLoadedAll)
            {
                var rules = await _ruleRepository.GetAsync(token);

                foreach (var data in rules)
                {
                    var entity = _ruleDataMapper.MapToDomain(data);
                    var tracked = _buffer.FirstOrDefault(x => x.Entity.Id == entity.Id);

                    if (tracked is null)
                    {
                        _buffer.Add(new TrackedEntity<Rule>
                        {
                            Entity = entity,
                            State = EntityState.Unchanged
                        });
                    }
                }

                _isLoadedAll = true;
            }

            return _buffer
                .Where(x => x.State != EntityState.Deleted)
                .Select(te => te.Entity)
                .ToList();
        }

        public async Task<Rule?> GetByIdAsync(Guid id, CancellationToken token)
        {
            var tracked = _buffer.FirstOrDefault(r => r.Entity.Id == id);

            if (tracked is not null)
            {
                return tracked.State == EntityState.Deleted ? null : tracked.Entity;
            }

            var data = await _ruleRepository.GetByIdAsync(id, token);
            if (data is null) return null;

            var rule = _ruleDataMapper.MapToDomain(data);

            _buffer.Add(new TrackedEntity<Rule>
            {
                Entity = rule,
                State = EntityState.Unchanged
            });

            return rule;
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
            {
                _buffer.Add(new TrackedEntity<Rule>
                {
                    Entity = rule,
                    State = EntityState.Modified
                });
                return true;
            }

            if (item.State == EntityState.Deleted)
                return false;

            item.Entity = rule;
            if (item.State != EntityState.Added)
                item.State = EntityState.Modified;

            return true;
        }

        public bool Delete(Guid id)
        {
            var item = _buffer.FirstOrDefault(x => x.Entity.Id == id);

            if (item is null)
            {
                var stubEntity = new Rule { Id = id };
                _buffer.Add(new TrackedEntity<Rule>
                {
                    Entity = stubEntity,
                    State = EntityState.Deleted
                });
                return true;
            }

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
                        await _ruleRepository.AddAsync(_ruleDataMapper.MapToData(item.Entity), token);
                        break;
                    case EntityState.Modified:
                        await _ruleRepository.UpdateAsync(_ruleDataMapper.MapToData(item.Entity), token);
                        break;
                    case EntityState.Deleted:
                        await _ruleRepository.DeleteAsync(item.Entity.Id, token);
                        break;
                }
            }

            await _rulePersistence.SaveAsync(token);

            _buffer.RemoveAll(x => x.State == EntityState.Deleted);

            foreach (var item in _buffer)
                item.State = EntityState.Unchanged;
        }
    }
}
