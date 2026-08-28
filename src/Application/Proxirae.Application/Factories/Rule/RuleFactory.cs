using Proxirae.Contracts.DTOs.Rules;
using Proxirae.Contracts.DTOs.Rules.Actions;

namespace Proxirae.Application.Factories.Rule
{
    public static class RuleFactory
    {
        public static RuleAddDto CreateAddDto(
            int priority,
            bool isEnabled,
            string? remarks,
            string? processes,
            string? hosts,
            string? ports,
            IEnumerable<RuleProtocolDto> selectedProtocols,
            BaseActionDto selectedAction)
        {
            return new RuleAddDto
            {
                Priority = priority,
                IsEnabled = isEnabled,
                Remarks = remarks,
                Processes = processes,
                Hosts = hosts,
                Ports = ports,
                Protocol = selectedProtocols.Any()
                    ? selectedProtocols.Aggregate((acc, p) => acc | p)
                    : RuleProtocolDto.None,
                Action = selectedAction
            };
        }

        public static RuleEditDto CreateEditDto(RuleDto rule)
        {
            return new RuleEditDto
            {
                Id = rule.Id,
                Priority = rule.Priority,
                IsEnabled = rule.IsEnabled,
                Remarks = rule.Remarks,
                Processes = rule.Processes,
                Hosts = rule.Hosts,
                Ports = rule.Ports,
                Protocol = rule.Protocol,
                Action = rule.Action
            };
        }

        public static RuleEditDto CreateEditDto(
            Guid id,
            int priority,
            bool isEnabled,
            string? remarks,
            string? processes,
            string? hosts,
            string? ports,
            IEnumerable<RuleProtocolDto> selectedProtocols,
            BaseActionDto selectedAction)
        {
            return new RuleEditDto
            {
                Id = id,
                Priority = priority,
                IsEnabled = isEnabled,
                Remarks = remarks,
                Processes = processes,
                Hosts = hosts,
                Ports = ports,
                Protocol = selectedProtocols.Any()
                    ? selectedProtocols.Aggregate((acc, p) => acc | p)
                    : RuleProtocolDto.None,
                Action = selectedAction
            };
        }
    }
}
