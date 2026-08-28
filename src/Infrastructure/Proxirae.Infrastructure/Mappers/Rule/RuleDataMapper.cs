using Proxirae.Application.Mappers.Rules;
using Proxirae.Application.Mappers.Rules.Actions;
using Proxirae.Contracts.Data.Rules;
using Proxirae.Domain.Rules;

namespace Proxirae.Infrastructure.Mappers.Rule
{
    public class RuleDataMapper : IRuleDataMapper
    {
        private readonly IActionDataMapper _actionDataMapper;

        public RuleDataMapper(IActionDataMapper actionDataMapper)
        {
            _actionDataMapper = actionDataMapper;
        }

        public RuleData MapToData(Domain.Rules.Rule rule)
        {
            return new RuleData
            {
                Id = rule.Id,
                Priority = rule.Priority,
                IsEnabled = rule.IsEnabled,
                Remarks = rule.Remarks,
                Processes = rule.Processes,
                Hosts = rule.Hosts,
                Ports = rule.Ports,
                Protocol = (RuleDataProtocol)rule.Protocol,
                Action = _actionDataMapper.MapToData(rule.Action)
            };
        }

        public Domain.Rules.Rule MapToDomain(RuleData data)
        {
            return new Domain.Rules.Rule
            {
                Id = data.Id,
                Priority = data.Priority,
                IsEnabled = data.IsEnabled,
                Remarks = data.Remarks,
                Processes = data.Processes,
                Hosts = data.Hosts,
                Ports = data.Ports,
                Protocol = (RuleProtocol)data.Protocol,
                Action = _actionDataMapper.MapToDomain(data.Action)
            };
        }
    }
}
