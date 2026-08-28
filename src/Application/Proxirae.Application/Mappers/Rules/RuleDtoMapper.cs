using Proxirae.Application.Mappers.Rules.Actions;
using Proxirae.Contracts.DTOs.Rules;
using Proxirae.Domain.Rules;

namespace Proxirae.Application.Mappers.Rules
{
    public class RuleDtoMapper : IRuleDtoMapper
    {
        private readonly IActionDtoMapper _actionMapper;

        public RuleDtoMapper(IActionDtoMapper actionMapper)
        {
            _actionMapper = actionMapper;
        }

        public RuleDto MapToDto(Rule rule)
        {
            return new RuleDto
            {
                Id = rule.Id,
                Priority = rule.Priority,
                IsEnabled = rule.IsEnabled,
                Remarks = rule.Remarks,
                Processes = rule.Processes,
                Hosts = rule.Hosts,
                Ports = rule.Ports,
                Protocol = (RuleProtocolDto)rule.Protocol,
                Action = _actionMapper.MapToDto(rule.Action)
            };
        }

        public Rule MapToDomain(RuleAddDto ruleAddDto)
        {
            return new Rule
            {
                Id = Guid.NewGuid(),
                Priority = ruleAddDto.Priority,
                IsEnabled = ruleAddDto.IsEnabled,
                Remarks = ruleAddDto.Remarks,
                Processes = ruleAddDto.Processes,
                Hosts = ruleAddDto.Hosts,
                Ports = ruleAddDto.Ports,
                Protocol = (RuleProtocol)ruleAddDto.Protocol,
                Action = _actionMapper.MapToDomain(ruleAddDto.Action)
            };
        }

        public Rule MapToDomain(RuleEditDto ruleEditDto)
        {
            return new Rule
            {
                Id = ruleEditDto.Id,
                Priority = ruleEditDto.Priority,
                IsEnabled = ruleEditDto.IsEnabled,
                Remarks = ruleEditDto.Remarks,
                Processes = ruleEditDto.Processes,
                Hosts = ruleEditDto.Hosts,
                Ports = ruleEditDto.Ports,
                Protocol = (RuleProtocol)ruleEditDto.Protocol,
                Action = _actionMapper.MapToDomain(ruleEditDto.Action)
            };
        }
    }
}
