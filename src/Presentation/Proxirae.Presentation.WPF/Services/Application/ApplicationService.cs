using Proxirae.Application.Services.Application;

namespace Proxirae.Presentation.WPF.Services.Application
{
    internal class ApplicationService : IApplicationService
    {
        public void Restore()
        {
            WinApp.Current.Dispatcher.Invoke(() =>
            {
                var window = WinApp.Current.MainWindow;
                if (window is not null)
                {
                    window.Show();
                    window.WindowState = System.Windows.WindowState.Normal;
                    window.Activate();
                }
            });
        }

        public void Shutdown()
        {
            WinApp.Current.Dispatcher.Invoke(WinApp.Current.Shutdown);
        }
    }
}
