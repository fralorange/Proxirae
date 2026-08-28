using Proxirae.Application.Mappers.Rules.Actions;
using Proxirae.Contracts.Data.Rules.Actions;
using Proxirae.Domain.Rules.Actions;

namespace Proxirae.Infrastructure.Mappers.Rule.Action
{
    public class ActionDataMapper : IActionDataMapper
    {
        public ActionData MapToData(BaseAction action)
        {
            return action switch
            {
                DirectAction directAction => new ActionData { Name = directAction.Name },
                BlockAction blockAction => new ActionData { Name = blockAction.Name },
                ProxyAction proxyAction => new ActionData { Name = proxyAction.Name, ProxyId = proxyAction.Id },
                _ => throw new NotSupportedException($"Unknown action type: {action.GetType().Name}")
            };
        }

        public BaseAction MapToDomain(ActionData action)
        {
            return action switch
            {
                { Name: "Direct" } => new DirectAction(),
                { Name: "Block" } => new BlockAction(),
                { ProxyId: not null } proxyAction => new ProxyAction(proxyAction.Name, proxyAction.ProxyId.Value),
                _ => throw new NotSupportedException($"Unknown action type: {action.GetType().Name}")
            };
        }
    }
}
