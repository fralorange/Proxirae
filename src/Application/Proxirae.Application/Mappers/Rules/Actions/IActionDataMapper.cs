using Proxirae.Contracts.Data.Rules.Actions;
using Proxirae.Domain.Rules.Actions;

namespace Proxirae.Application.Mappers.Rules.Actions
{
    public interface IActionDataMapper
    {
        ActionData MapToData(BaseAction action);
        BaseAction MapToDomain(ActionData action);
    }
}
