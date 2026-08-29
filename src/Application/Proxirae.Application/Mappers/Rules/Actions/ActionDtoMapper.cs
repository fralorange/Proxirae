using Proxirae.Application.Mappers.Proxies;
using Proxirae.Contracts.DTOs.Rules.Actions;
using Proxirae.Domain.Rules.Actions;

namespace Proxirae.Application.Mappers.Rules.Actions
{
    public class ActionDtoMapper : IActionDtoMapper
    {
        public BaseActionDto MapToDto(BaseAction action)
        {
            return action switch
            {
                DirectAction directAction => new DirectActionDto() { Id = directAction.Id },
                BlockAction blockAction => new BlockActionDto() { Id = blockAction.Id},
                ProxyAction proxyAction => new ProxyActionDto(proxyAction.Name, proxyAction.Id),
                _ => throw new NotSupportedException($"Unknown action type: {action.GetType().Name}")
            };
        }

        public BaseAction MapToDomain(BaseActionDto action)
        {
            return action switch
            {
                DirectActionDto => new DirectAction(),
                BlockActionDto => new BlockAction(),
                ProxyActionDto proxyAction => new ProxyAction(proxyAction.Name, proxyAction.Id),
                _ => throw new NotSupportedException($"Unknown action type: {action.GetType().Name}")
            };
        }
    }
}
