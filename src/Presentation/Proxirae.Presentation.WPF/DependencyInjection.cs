using Microsoft.Extensions.DependencyInjection;
using MvvmDialogs;
using Proxirae.Application.Mappers.Proxy;
using Proxirae.Application.Services.Application;
using Proxirae.Application.Services.Proxy;
using Proxirae.Presentation.WPF.Facades.Dialog;
using Proxirae.Presentation.WPF.Services.Application;
using Proxirae.Presentation.WPF.ViewModels;

namespace Proxirae.Presentation.WPF
{
    public static class DependencyInjection
    {
        public static IServiceCollection AddStartupServices(this IServiceCollection services)
        {
            services.AddSingleton<MainView>();
            services.AddSingleton<MainViewModel>();
            return services;
        }

        public static IServiceCollection AddServices(this IServiceCollection services)
        {
            services.AddTransient<IApplicationService, ApplicationService>();
            services.AddTransient<IProxyService, ProxyService>();
            services.AddSingleton<IDialogService>(new DialogService(dialogTypeLocator: new TypeLocator()));
            return services;
        }

        public static IServiceCollection AddMappers(this IServiceCollection services)
        {
            services.AddTransient<IProxyMapper, ProxyMapper>();
            return services;
        }

        public static IServiceCollection AddFacades(this IServiceCollection services)
        {
            services.AddTransient<DialogFacade>();
            return services;
        }

        public static IServiceCollection AddViewModels(this IServiceCollection services)
        {
            services.AddTransient<ProxyServersViewModel>();
            services.AddTransient<AddProxyServerViewModel>();
            services.AddTransient<EditProxyServerViewModel>();
            services.AddTransient<ProxyRulesViewModel>();
            return services;
        }
    }
}
