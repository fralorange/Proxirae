using Proxirae.Application.Mappers.Rules;
using Proxirae.Application.Messenger;
using Proxirae.Application.UnitsOfWork.Rules;
using Proxirae.Contracts.DTOs.Rules;

namespace Proxirae.Application.Services.Rules
{
    public class RuleService : IRuleService
    {
        private readonly IRuleUnitOfWork _ruleUnitOfWork;
        private readonly IRuleDtoMapper _ruleMapper;
        private readonly IMessenger _messenger;

        public RuleService(IRuleUnitOfWork ruleUnitOfWork, IRuleDtoMapper ruleMapper, IMessenger messenger)
        {
            _ruleUnitOfWork = ruleUnitOfWork;
            _ruleMapper = ruleMapper;
            _messenger = messenger;
        }

        public async Task<IReadOnlyCollection<RuleDto>> GetAsync(CancellationToken token)
        {
            var rules = await _ruleUnitOfWork.GetAsync(token);
            return rules.Select(_ruleMapper.MapToDto).ToList();
        }

        public async Task<RuleDto?> GetByIdAsync(Guid id, CancellationToken token)
        {
            var rule = await _ruleUnitOfWork.GetByIdAsync(id, token);
            return rule is null ? null : _ruleMapper.MapToDto(rule);
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

        public async Task SwapPrioritiesAsync(Guid sourceId, Guid targetId, CancellationToken token)
        {
            var sourceRule = await _ruleUnitOfWork.GetByIdAsync(sourceId, token);
            var targetRule = await _ruleUnitOfWork.GetByIdAsync(targetId, token);

            if (sourceRule is null || targetRule is null)
            {
                return;
            }

            (sourceRule.Priority, targetRule.Priority) = (targetRule.Priority, sourceRule.Priority);

            _ruleUnitOfWork.Update(sourceRule);
            _ruleUnitOfWork.Update(targetRule);
        }

        public async Task SaveChangesAsync(CancellationToken token)
        {
            await _ruleUnitOfWork.SaveChangesAsync(token);

            await _messenger.SendMessageAsync(MessageType.Cmd_ReloadRules, token);
        }
    }
}
