using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using Proxirae.Application.Services.Application;
using Proxirae.Presentation.WPF.Services.Dialog;

namespace Proxirae.Presentation.WPF.ViewModels
{
    public partial class MainViewModel : ObservableObject
    {
        private readonly IApplicationService _applicationService;
        private readonly IDialogService _dialogService;

        public MainViewModel(IApplicationService applicationService, IDialogService dialogService)
        {
            _applicationService = applicationService;
            _dialogService = dialogService;
        }

        [RelayCommand]
        private void Exit()
        {
            _applicationService.Shutdown();
        }

        [RelayCommand]
        private void OpenProxyServers()
        {
            _dialogService.Show<ProxyServersViewModel>();
        }
    }
}
