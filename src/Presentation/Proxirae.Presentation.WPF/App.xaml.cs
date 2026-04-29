using Microsoft.Extensions.DependencyInjection;
using System.Windows;
using System.Windows.Controls;

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
                .BuildServiceProvider();
        }

        private static void DisableTabStop()
        {
            Control.IsTabStopProperty.OverrideMetadata(
                typeof(Control),
                new FrameworkPropertyMetadata(false));
        }

        protected override void OnStartup(StartupEventArgs e)
        {
            DisableTabStop();

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
