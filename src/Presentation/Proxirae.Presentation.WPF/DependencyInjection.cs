using Microsoft.Extensions.DependencyInjection;
using MvvmDialogs;
using Proxirae.Application.Facades.Actions;
using Proxirae.Application.Facades.Routes;
using Proxirae.Application.Mappers.Proxies;
using Proxirae.Application.Mappers.Rules;
using Proxirae.Application.Mappers.Rules.Actions;
using Proxirae.Application.Services.Application;
using Proxirae.Application.Services.Archive;
using Proxirae.Application.Services.Browser;
using Proxirae.Application.Services.Clipboard;
using Proxirae.Application.Services.Configuration;
using Proxirae.Application.Services.Flows;
using Proxirae.Application.Services.Localization;
using Proxirae.Application.Services.Logs;
using Proxirae.Application.Services.Package;
using Proxirae.Application.Services.Preferences;
using Proxirae.Application.Services.Preferences.Autostart;
using Proxirae.Application.Services.Proxies;
using Proxirae.Application.Services.Routes;
using Proxirae.Application.Services.Rules;
using Proxirae.Application.Services.Test;
using Proxirae.Application.Services.UI;
using Proxirae.Application.Validators.ProxyRule;
using Proxirae.Infrastructure;
using Proxirae.Infrastructure.Mappers.Rule;
using Proxirae.Infrastructure.Mappers.Rule.Action;
using Proxirae.Infrastructure.Services.Configuration;
using Proxirae.Presentation.WPF.Factories.Flow;
using Proxirae.Presentation.WPF.Factories.ProxyChecker;
using Proxirae.Presentation.WPF.Factories.Routes;
using Proxirae.Presentation.WPF.Services.Application;
using Proxirae.Presentation.WPF.Services.Archive;
using Proxirae.Presentation.WPF.Services.Browser;
using Proxirae.Presentation.WPF.Services.Clipboard;
using Proxirae.Presentation.WPF.Services.Dialog.File;
using Proxirae.Presentation.WPF.Services.Dialog.Input;
using Proxirae.Presentation.WPF.Services.Dialog.Message;
using Proxirae.Presentation.WPF.Services.Dialog.Modal;
using Proxirae.Presentation.WPF.Services.Localization;
using Proxirae.Presentation.WPF.Services.Preferences.Autostart;
using Proxirae.Presentation.WPF.Services.Process;
using Proxirae.Presentation.WPF.Services.Themes;
using Proxirae.Presentation.WPF.Services.UI;
using Proxirae.Presentation.WPF.ViewModels;
using Proxirae.Presentation.WPF.ViewModels.About;
using Proxirae.Presentation.WPF.ViewModels.Options;
using Proxirae.Presentation.WPF.ViewModels.Options.Sections;
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
            services.AddSingleton<IPreferencesService, PreferencesService>();
            services.AddHostedService<PreferencesHostedService>();
            services.AddSingleton<ITestService, TestService>();
            services.AddSingleton<IAutostartService, WindowsAutostartService>();
            services.AddSingleton<IProcessInfoService, ProcessInfoService>();
            services.AddTransient<IBrowserService, BrowserService>();
            services.AddTransient<IUiService, WpfUiService>();
            services.AddTransient<IArchiveService, ArchiveService>();
            services.AddSingleton<ILocalizationService, LocalizationService>();
            services.AddSingleton<IThemeService, WpfThemeService>();
            services.AddSingleton<IModalDialogService, ModalDialogService>();
            services.AddTransient<IFileDialogService, FileDialogService>();
            services.AddSingleton<IMessageDialogService, MessageDialogService>();
            services.AddSingleton<IInputDialogService, InputDialogService>();
            services.AddTransient<IConfigurationService, ConfigurationService>();
            services.AddTransient<IPackageService, PackageService>();

            return services;
        }

        public static IServiceCollection AddValidators(this IServiceCollection services)
        {
            services.AddTransient<IProxyRuleValidator, RuleService>();

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
            services.AddSingleton<ActionFacade>();
            services.AddSingleton<RouteFacade>();

            return services;
        }

        public static IServiceCollection AddFactories(this IServiceCollection services)
        {
            services.AddTransient<IProxyCheckerViewModelFactory, ProxyCheckerViewModelFactory>();
            services.AddTransient<IFlowViewModelFactory, FlowViewModelFactory>();
            services.AddTransient<IRouteViewModelFactory, RouteViewModelFactory>();

            return services;
        }

        public static IServiceCollection AddViewModels(this IServiceCollection services)
        {
            services.AddTransient<ProxyServersViewModel>();
            services.AddTransient<AddProxyServerViewModel>();
            services.AddTransient<ProxyRulesViewModel>();
            services.AddTransient<AddProxyRuleViewModel>();
            services.AddTransient<ProxyCheckerViewModel>();
            services.AddTransient<AboutViewModel>();
            services.AddTransient<OptionsViewModel>();
            services.AddTransient<GeneralViewModel>();
            services.AddTransient<AppearanceViewModel>();
            services.AddTransient<MetricsViewModel>();

            return services;
        }

        public static IServiceCollection AddCache(this IServiceCollection services)
        {
            services.AddMemoryCache(options =>
            {
                options.SizeLimit = 500;
            });

            return services;
        }
    }
}
