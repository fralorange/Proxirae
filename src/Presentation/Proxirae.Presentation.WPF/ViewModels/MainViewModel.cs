using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using Proxirae.Application.Services.Application;
using Proxirae.Presentation.WPF.Facades.Dialog;

namespace Proxirae.Presentation.WPF.ViewModels
{
    public partial class MainViewModel : ObservableObject
    {
        private readonly IApplicationService _applicationService;
        private readonly DialogFacade _dialogFacade;

        public MainViewModel(IApplicationService applicationService, DialogFacade dialogFacade)
        {
            _applicationService = applicationService;
            _dialogFacade = dialogFacade;
        }

        [RelayCommand]
        private void Exit()
        {
            _applicationService.Shutdown();
        }

        [RelayCommand]
        private void OpenProxyServers()
        {
            _dialogFacade.ShowDialog<ProxyServersViewModel>(this);
        }

        [RelayCommand]
        private void OpenProxyRules()
        {
            _dialogFacade.ShowDialog<ProxyRulesViewModel>(this);
        }
    }
}
