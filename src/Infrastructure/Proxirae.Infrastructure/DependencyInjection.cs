using Microsoft.Extensions.DependencyInjection;
using Proxirae.Application.Repositories.Proxy;
using Proxirae.Infrastructure.Repositories.Proxy;

namespace Proxirae.Infrastructure
{
    public static class DependencyInjection
    {
        public static IServiceCollection AddRepositories(this IServiceCollection services)
        {
            services.AddSingleton<IProxyRepository, InMemoryProxyRepository>();
            return services;
        }
    }
}
