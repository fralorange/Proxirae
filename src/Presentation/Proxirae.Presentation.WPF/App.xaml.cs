using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Proxirae.Application.Services.Preferences;
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
                        .AddServices()
                        .AddRepositories()
                        .AddStores()
                        .AddUnitsOfWork()
                        .AddMappers()
                        .AddFacades()
                        .AddFactories()
                        .AddViewModels()
                        .AddPipes();
                })
                .Build();
        }

        protected override async void OnStartup(StartupEventArgs e)
        {
            await _host.StartAsync();

            var mainView = _host.Services.GetRequiredService<MainView>();
            mainView.Show();
            Current.MainWindow = mainView;

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
