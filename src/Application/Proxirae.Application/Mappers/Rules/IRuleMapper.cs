using Proxirae.Contracts.DTOs.Rules;
using Proxirae.Domain.Rules;

namespace Proxirae.Application.Mappers.Rules
{
    public interface IRuleMapper
    {
        RuleDto MapToDto(Rule rule);
        Rule MapToDomain(RuleAddDto ruleAddDto);
        Rule MapToDomain(RuleEditDto ruleEditDto);
    }
}
