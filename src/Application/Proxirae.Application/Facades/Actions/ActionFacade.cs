using Microsoft.Extensions.DependencyInjection;
using Proxirae.Application.Mappers.Rules.Actions;
using Proxirae.Application.Repositories;
using Proxirae.Contracts.DTOs.Rules.Actions;
using Proxirae.Domain.Proxies;
using Proxirae.Domain.Rules.Actions;

namespace Proxirae.Application.Facades.Actions
{
    public class ActionFacade
    {
        private readonly IActionDtoMapper _actionMapper;
        private readonly IServiceScopeFactory _scopeFactory;

        public ActionFacade(IActionDtoMapper actionMapper, IServiceScopeFactory scopeFactory)
        {
            _actionMapper = actionMapper;
            _scopeFactory = scopeFactory;
        }

        public async Task<List<BaseActionDto>> GetActionsAsync(CancellationToken token)
        {
            using var scope = _scopeFactory.CreateAsyncScope();

            var proxyRepository = scope.ServiceProvider.GetRequiredService<ICrudRepository<Proxy>>();

            var proxies = await proxyRepository.GetAsync(token);

            var actions = new List<BaseAction>()
            {
                new DirectAction(),
                new BlockAction(),
            };

            actions.AddRange(proxies.Select(ProxyAction.CreateFrom));
            return actions.Select(_actionMapper.MapToDto).ToList();
        }

        public async Task<BaseActionDto> GetActionAsync(Guid proxyId, CancellationToken token)
        {
            using var scope = _scopeFactory.CreateAsyncScope();

            var proxyRepository = scope.ServiceProvider.GetRequiredService<ICrudRepository<Proxy>>();

            var proxy = await proxyRepository.GetByIdAsync(proxyId, token);

            if (proxy is null)
            {
                return _actionMapper.MapToDto(new DirectAction());
            }

            var proxyAction = ProxyAction.CreateFrom(proxy);
            return _actionMapper.MapToDto(proxyAction);
        }
    }
}
