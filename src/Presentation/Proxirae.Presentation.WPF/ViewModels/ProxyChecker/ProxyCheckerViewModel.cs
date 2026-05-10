using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using MvvmDialogs;
using Proxirae.Contracts.DTOs.Proxies;
using Proxirae.Presentation.WPF.Facades.Dialog;

namespace Proxirae.Presentation.WPF.ViewModels.ProxyChecker
{
    public partial class ProxyCheckerViewModel : ObservableObject, IModalDialogViewModel
    {
        private readonly DialogFacade _dialogFacade;

        private bool? _dialogResult;

        public bool? DialogResult
        {
            get => _dialogResult;
            protected set => SetProperty(ref _dialogResult, value);
        }

        [ObservableProperty]
        [NotifyPropertyChangedFor(nameof(IsProxySet))]
        private ProxyDetailDto? _proxyServer;

        public bool IsProxySet => ProxyServer is not null;

        public ProxyCheckerViewModel(DialogFacade dialogFacade)
        {
            _dialogFacade = dialogFacade;
        }

        public ProxyCheckerViewModel(DialogFacade dialogFacade, ProxyDetailDto proxy)
        {
            _dialogFacade = dialogFacade;
            ProxyServer = proxy;
        }

        [RelayCommand]
        private void SetProxyServer()
        {
            var viewModel = new SetProxyServerViewModel(ProxyServer);
            _dialogFacade.ShowDialog(this, viewModel);

            if (viewModel.ProxyServer is not null)
            {
                ProxyServer = viewModel.ProxyServer;
            }
        }
    }
}
