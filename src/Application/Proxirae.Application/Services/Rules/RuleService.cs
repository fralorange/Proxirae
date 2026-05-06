using Proxirae.Application.Mappers.Rules;
using Proxirae.Application.UnitsOfWork.Rules;
using Proxirae.Contracts.DTOs.Rules;

namespace Proxirae.Application.Services.Rules
{
    public class RuleService : IRuleService
    {
        private readonly IRuleUnitOfWork _ruleUnitOfWork;
        private readonly IRuleMapper _ruleMapper;

        public RuleService(IRuleUnitOfWork ruleUnitOfWork, IRuleMapper ruleMapper)
        {
            _ruleUnitOfWork = ruleUnitOfWork;
            _ruleMapper = ruleMapper;
        }

        public async Task<IReadOnlyCollection<RuleDto>> GetAsync(CancellationToken token)
        {
            var rules = await _ruleUnitOfWork.GetAsync(token);
            return rules.Select(_ruleMapper.MapToDto).ToList();
        }

        public Task<RuleDto?> GetByIdAsync(Guid id, CancellationToken token)
        {
            var rule = _ruleUnitOfWork.GetById(id);
            return Task.FromResult(rule is null ? null : _ruleMapper.MapToDto(rule));
        }

        public Task<RuleDto> AddAsync(RuleAddDto ruleAddDto, CancellationToken token)
        {
            var rule = _ruleMapper.MapToDomain(ruleAddDto);
            _ruleUnitOfWork.Add(rule);
            return Task.FromResult(_ruleMapper.MapToDto(rule));
        }

        public Task<RuleDto?> UpdateAsync(RuleEditDto ruleEditDto, CancellationToken token)
        {
            var rule = _ruleMapper.MapToDomain(ruleEditDto);
            var result = _ruleUnitOfWork.Update(rule);
            if (result)
            {
                return Task.FromResult<RuleDto?>(_ruleMapper.MapToDto(rule));
            }
            else
            {
                return Task.FromResult<RuleDto?>(null);
            }
        }

        public Task<bool> DeleteAsync(Guid id, CancellationToken token)
        {
            return Task.FromResult(_ruleUnitOfWork.Delete(id));
        }

        public Task SaveChangesAsync(CancellationToken token)
        {
            return _ruleUnitOfWork.SaveChangesAsync(token);
        }
    }
}
