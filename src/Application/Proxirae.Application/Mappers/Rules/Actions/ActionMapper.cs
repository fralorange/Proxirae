using Proxirae.Application.Mappers.Proxies;
using Proxirae.Contracts.DTOs.Rules.Actions;
using Proxirae.Domain.Rules.Actions;

namespace Proxirae.Application.Mappers.Rules.Actions
{
    public class ActionMapper : IActionMapper
    {
        private readonly IProxyMapper _proxyMapper;

        public ActionMapper(IProxyMapper proxyMapper)
        {
            _proxyMapper = proxyMapper;
        }

        public BaseActionDto MapToDto(BaseAction action)
        {
            return action switch
            {
                DirectAction directAction => new DirectActionDto() { Id = directAction.Id },
                BlockAction blockAction => new BlockActionDto() { Id = blockAction.Id},
                ProxyAction proxyAction => new ProxyActionDto(proxyAction.Name, _proxyMapper.MapToDetail(proxyAction.Proxy)),
                _ => throw new NotSupportedException($"Unknown action type: {action.GetType().Name}")
            };
        }

        public BaseAction MapToDomain(BaseActionDto action)
        {
            return action switch
            {
                DirectActionDto directAction => new DirectAction(),
                BlockActionDto blockAction => new BlockAction(),
                ProxyActionDto proxyAction => new ProxyAction(_proxyMapper.MapToDomain(proxyAction.Proxy)),
                _ => throw new NotSupportedException($"Unknown action type: {action.GetType().Name}")
            };
        }
    }
}
