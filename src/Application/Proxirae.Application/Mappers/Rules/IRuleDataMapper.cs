using Proxirae.Contracts.Data.Rules;
using Proxirae.Domain.Rules;

namespace Proxirae.Application.Mappers.Rules
{
    public interface IRuleDataMapper
    {
        RuleData MapToData(Rule rule);
        Rule MapToDomain(RuleData data);
    }
}
