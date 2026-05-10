using Microsoft.Extensions.DependencyInjection;
using MvvmDialogs;
using Proxirae.Application.Facades.Actions;
using Proxirae.Application.Mappers.Proxies;
using Proxirae.Application.Mappers.Rules;
using Proxirae.Application.Mappers.Rules.Actions;
using Proxirae.Application.Services.Application;
using Proxirae.Application.Services.Proxies;
using Proxirae.Application.Services.Rules;
using Proxirae.Presentation.WPF.Facades.Dialog;
using Proxirae.Presentation.WPF.Services.Application;
using Proxirae.Presentation.WPF.ViewModels;
using Proxirae.Presentation.WPF.ViewModels.ProxyChecker;
using Proxirae.Presentation.WPF.ViewModels.ProxyRules;
using Proxirae.Presentation.WPF.ViewModels.ProxyServers;

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
            services.AddTransient<IRuleService, RuleService>();
            services.AddSingleton<IDialogService>(new DialogService(dialogTypeLocator: new TypeLocator()));
            return services;
        }

        public static IServiceCollection AddMappers(this IServiceCollection services)
        {
            services.AddTransient<IProxyMapper, ProxyMapper>();
            services.AddTransient<IRuleMapper, RuleMapper>();
            services.AddTransient<IActionMapper, ActionMapper>();
            return services;
        }

        public static IServiceCollection AddFacades(this IServiceCollection services)
        {
            services.AddTransient<DialogFacade>();
            services.AddTransient<ActionFacade>();
            return services;
        }

        public static IServiceCollection AddViewModels(this IServiceCollection services)
        {
            services.AddTransient<ProxyServersViewModel>();
            services.AddTransient<AddProxyServerViewModel>();
            services.AddTransient<ProxyRulesViewModel>();
            services.AddTransient<AddProxyRuleViewModel>();
            services.AddTransient<ProxyCheckerViewModel>();
            return services;
        }
    }
}
