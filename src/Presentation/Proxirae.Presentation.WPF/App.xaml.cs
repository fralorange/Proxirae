using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Proxirae.Application.Services.UI;
using Proxirae.Infrastructure;
using Proxirae.Infrastructure.ProcessCommunication;
using Proxirae.Infrastructure.TransactionControl;
using System.Windows;

namespace Proxirae.Presentation.WPF
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : WinApp
    {
        private readonly IHost _host;

        public App()
        {
            _host = Host.CreateDefaultBuilder()
                .ConfigureServices(services =>
                {
                    services
                        .AddStartupServices()
                        .AddCache()
                        .AddServices()
                        .AddRepositories()
                        .AddStores()
                        .AddExporters()
                        .AddUnitsOfWork()
                        .AddValidators()
                        .AddMappers()
                        .AddFacades()
                        .AddFactories()
                        .AddViewModels()
                        .AddCommunication();
                })
                .Build();
        }

        protected override async void OnStartup(StartupEventArgs e)
        {
            await _host.StartAsync();

            ShutdownMode = ShutdownMode.OnExplicitShutdown;

            var uiConfigurator = _host.Services.GetRequiredService<IUiService>();
            uiConfigurator.ApplyGlobalTweaks();

            var mainView = _host.Services.GetRequiredService<MainView>();
            Current.MainWindow = mainView;
            mainView.Show();

            base.OnStartup(e);
        }

        protected override async void OnExit(ExitEventArgs e)
        {
            await _host.StopAsync();
            _host.Dispose();

            base.OnExit(e);
        }
    }
}
