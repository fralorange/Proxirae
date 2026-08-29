using Microsoft.Extensions.DependencyInjection;
using Proxirae.Application.Services.Rules;

namespace Proxirae.Application.Facades.Routes
{
    public class RouteFacade
    {
        private readonly IServiceScopeFactory _scopeFactory;

        public RouteFacade(IServiceScopeFactory scopeFactory)
        {
            _scopeFactory = scopeFactory;
        }

        public async Task<string?> GetActionNameAsync(Guid ruleId, CancellationToken cancellationToken)
        {
            using var scope = _scopeFactory.CreateAsyncScope();

            var ruleService = scope.ServiceProvider.GetRequiredService<IRuleService>();

            var rule = await ruleService.GetByIdAsync(ruleId, cancellationToken);

            if (rule is null)
            {
                return null;
            }

            return rule.Action.Name;
        }
    }
}
