using Microsoft.Extensions.DependencyInjection;
using Proxirae.Application.Repositories.Proxies;
using Proxirae.Application.Repositories.Rules;
using Proxirae.Infrastructure.Repositories.Proxies;
using Proxirae.Infrastructure.Repositories.Rules;

namespace Proxirae.Infrastructure
{
    public static class DependencyInjection
    {
        public static IServiceCollection AddRepositories(this IServiceCollection services)
        {
            services.AddSingleton<IProxyRepository, InMemoryProxyRepository>();
            services.AddSingleton<IRuleRepository, InMemoryRuleRepository>();
            return services;
        }
    }
}
