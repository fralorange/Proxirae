using Proxirae.Contracts.DTOs.Rules.Actions;
using Proxirae.Domain.Rules.Actions;

namespace Proxirae.Application.Mappers.Rules.Actions
{
    public interface IActionDtoMapper
    {
        BaseActionDto MapToDto(BaseAction action);
        BaseAction MapToDomain(BaseActionDto action);
    }
}
