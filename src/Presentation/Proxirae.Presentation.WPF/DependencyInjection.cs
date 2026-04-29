using Microsoft.Extensions.DependencyInjection;
using Proxirae.Application.Services.Application;
using Proxirae.Presentation.WPF.Services.Application;
using Proxirae.Presentation.WPF.Services.Dialog;
using Proxirae.Presentation.WPF.ViewModels;

namespace Proxirae.Presentation.WPF
{
    public static class DependencyInjection
    {
        public static IServiceCollection AddStartupServices(this IServiceCollection services)
        {
            services.AddSingleton<MainView>();
            services.AddSingleton<MainViewModel>();
            services.AddSingleton<ViewMappingRegistry>();
            return services;
        }

        public static IServiceCollection AddServices(this IServiceCollection services)
        {
            services.AddTransient<IApplicationService, ApplicationService>();
            services.AddTransient<IDialogService, DialogService>();
            return services;
        }
    }
}
