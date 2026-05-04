using Microsoft.Extensions.DependencyInjection;
using Proxirae.Application.UnitsOfWork.Proxy;
using Proxirae.Infrastructure.TransactionControl.UnitsOfWork;

namespace Proxirae.Infrastructure.TransactionControl
{
    public static class DependencyInjection
    {
        public static IServiceCollection AddUnitsOfWork(this IServiceCollection services)
        {
            services.AddTransient<IProxyUnitOfWork, ProxyUnitOfWork>();
            return services;
        }
    }
}
