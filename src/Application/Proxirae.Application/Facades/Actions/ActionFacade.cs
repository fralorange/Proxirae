using Proxirae.Application.Mappers.Rules.Actions;
using Proxirae.Application.Repositories.Proxies;
using Proxirae.Contracts.DTOs.Rules.Actions;
using Proxirae.Domain.Rules.Actions;

namespace Proxirae.Application.Facades.Actions
{
    public class ActionFacade
    {
        private readonly IProxyRepository _proxyRepository;
        private readonly IActionMapper _actionMapper;

        public ActionFacade(IProxyRepository proxyRepository, IActionMapper actionMapper)
        {
            _proxyRepository = proxyRepository;
            _actionMapper = actionMapper;
        }

        public async Task<List<BaseActionDto>> GetActionsAsync(CancellationToken token)
        {
            var proxies = await _proxyRepository.GetAsync(token);

            var actions = new List<BaseAction>()
            {
                new DirectAction(),
                new BlockAction(),
            };

            actions.AddRange(
                proxies.Select(p => new ProxyAction(p))
            );

            return actions.Select(_actionMapper.MapToDto).ToList();
        }
    }
}
