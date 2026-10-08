using CommunityToolkit.Mvvm.Input;
using H.NotifyIcon;
using Microsoft.Extensions.DependencyInjection;
using System.Windows;
using System.Windows.Controls;

namespace Proxirae.Presentation.WPF.Services.Tray
{
    public sealed class TrayService : IDisposable
    {
        private readonly IServiceProvider _serviceProvider;

        private TaskbarIcon? _trayIcon;
        private ContextMenu? _contextMenu;

        public RelayCommand OpenCommand { get; }
        public RelayCommand ExitCommand { get; }

        public TrayService(IServiceProvider serviceProvider)
        {
            _serviceProvider = serviceProvider;

            OpenCommand = new RelayCommand(Open);
            ExitCommand = new RelayCommand(Exit);
        }

        public void Initialize()
        {
            if (_trayIcon is not null)
            {
                return;
            }

            _trayIcon =
                (TaskbarIcon)WinApp.Current.Resources["TrayIcon"];

            _contextMenu = _trayIcon.ContextMenu;

            _trayIcon.DoubleClickCommand = OpenCommand;

            if (_contextMenu is not null)
            {
                if (_contextMenu.Items[0] is MenuItem openItem)
                {
                    openItem.Command = OpenCommand;
                }

                if (_contextMenu.Items[^1] is MenuItem exitItem)
                {
                    exitItem.Command = ExitCommand;
                }
            }

            _trayIcon.ForceCreate();
        }

        private void Open()
        {
            MainView mainView;

            if (WinApp.Current.MainWindow is MainView existingMainView)
            {
                mainView = existingMainView;
            }
            else
            {
                mainView = _serviceProvider.GetRequiredService<MainView>();
                WinApp.Current.MainWindow = mainView;
            }

            if (!mainView.IsVisible)
            {
                mainView.Show();
            }

            if (mainView.WindowState == WindowState.Minimized)
            {
                mainView.WindowState = WindowState.Normal;
            }

            mainView.Activate();
        }

        private static void Exit()
        {
            WinApp.Current.Shutdown();
        }

        public void Dispose()
        {
            _trayIcon?.Dispose();
            _trayIcon = null;
            _contextMenu = null;
        }
    }
}