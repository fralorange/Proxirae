using Microsoft.Extensions.DependencyInjection;
using MvvmDialogs;
using Proxirae.Application.Facades.Actions;
using Proxirae.Application.Facades.Routes;
using Proxirae.Application.Mappers.Proxies;
using Proxirae.Application.Mappers.Rules;
using Proxirae.Application.Mappers.Rules.Actions;
using Proxirae.Application.Services.Application;
using Proxirae.Application.Services.Clipboard;
using Proxirae.Application.Services.Flows;
using Proxirae.Application.Services.Logs;
using Proxirae.Application.Services.Preferences;
using Proxirae.Application.Services.Proxies;
using Proxirae.Application.Services.Routes;
using Proxirae.Application.Services.Rules;
using Proxirae.Application.Services.Test;
using Proxirae.Infrastructure.Mappers.Rule;
using Proxirae.Infrastructure.Mappers.Rule.Action;
using Proxirae.Presentation.WPF.Facades.Dialog;
using Proxirae.Presentation.WPF.Factories.ProxyChecker;
using Proxirae.Presentation.WPF.Services.Application;
using Proxirae.Presentation.WPF.Services.Clipboard;
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
            services.AddSingleton<IFlowService, FlowService>();
            services.AddSingleton<ILogService, LogService>();
            services.AddSingleton<IRouteService, RouteService>();
            services.AddTransient<IClipboardService, ClipboardService>();
            services.AddTransient<IPreferencesService, PreferencesService>();
            services.AddHostedService<PreferencesHostedService>();
            services.AddSingleton<ITestService, TestService>();
            return services;
        }

        public static IServiceCollection AddMappers(this IServiceCollection services)
        {
            services.AddTransient<IProxyDtoMapper, ProxyDtoMapper>();
            services.AddTransient<IRuleDtoMapper, RuleDtoMapper>();
            services.AddTransient<IActionDtoMapper, ActionDtoMapper>();
            services.AddTransient<IRuleDataMapper, RuleDataMapper>();
            services.AddTransient<IActionDataMapper, ActionDataMapper>();
            return services;
        }

        public static IServiceCollection AddFacades(this IServiceCollection services)
        {
            services.AddSingleton<DialogFacade>();
            services.AddSingleton<ActionFacade>();
            services.AddSingleton<RouteFacade>();
            return services;
        }

        public static IServiceCollection AddFactories(this IServiceCollection services)
        {
            services.AddTransient<IProxyCheckerViewModelFactory, ProxyCheckerViewModelFactory>();
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
