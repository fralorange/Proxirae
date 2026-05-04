using Microsoft.Extensions.DependencyInjection;
using Proxirae.Infrastructure;
using Proxirae.Infrastructure.TransactionControl;
using System.Windows;

namespace Proxirae.Presentation.WPF
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : WinApp
    {
        public IServiceProvider ServiceProvider { get; }

        public App()
        {
            ServiceProvider = new ServiceCollection()
                .AddStartupServices()
                .AddServices()
                .AddRepositories()
                .AddUnitsOfWork()
                .AddMappers()
                .AddFacades()
                .AddViewModels()
                .BuildServiceProvider();
        }

        protected override void OnStartup(StartupEventArgs e)
        {
            var mainView = ServiceProvider.GetRequiredService<MainView>();
            mainView.Show();
            Current.MainWindow = mainView;

            base.OnStartup(e);
        }

        protected override void OnExit(ExitEventArgs e)
        {
            if (ServiceProvider is IDisposable disposable)
            {
                disposable.Dispose();
            }

            base.OnExit(e);
        }
    }

}
