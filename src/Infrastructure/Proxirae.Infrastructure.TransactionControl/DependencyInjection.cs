using Microsoft.Extensions.DependencyInjection;
using Proxirae.Application.UnitsOfWork.Proxies;
using Proxirae.Application.UnitsOfWork.Rules;
using Proxirae.Infrastructure.TransactionControl.UnitsOfWork.Proxies;
using Proxirae.Infrastructure.TransactionControl.UnitsOfWork.Rules;

namespace Proxirae.Infrastructure.TransactionControl
{
    public static class DependencyInjection
    {
        public static IServiceCollection AddUnitsOfWork(this IServiceCollection services)
        {
            services.AddTransient<IProxyUnitOfWork, ProxyUnitOfWork>();
            services.AddTransient<IRuleUnitOfWork, RuleUnitOfWork>();
            return services;
        }
    }
}
