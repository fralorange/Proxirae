using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using MvvmDialogs;
using Proxirae.Application.Services.Proxy;
using Proxirae.Contracts.DTOs.Proxy;
using Proxirae.Presentation.WPF.Facades.Dialog;
using System.Collections.ObjectModel;
using System.Threading.Tasks;

namespace Proxirae.Presentation.WPF.ViewModels
{
    public partial class ProxyServersViewModel : ObservableObject, IModalDialogViewModel
    {
        private readonly DialogFacade _dialogFacade;
        private readonly IProxyService _proxyService;

        private bool? dialogResult;
        public bool? DialogResult
        {
            get => dialogResult;
            private set => SetProperty(ref dialogResult, value);
        }

        public ObservableCollection<ProxyListDto> ProxyServers { get; } = [];

        public ProxyServersViewModel(DialogFacade dialogFacade, IProxyService proxyService)
        {
            _dialogFacade = dialogFacade;
            _proxyService = proxyService;
        }

        [RelayCommand]
        private async Task LoadProxyServersAsync()
        {
            var proxyServers = await _proxyService.GetAsync(CancellationToken.None);

            ProxyServers.Clear();

            foreach (var proxyServer in proxyServers)
            {
                ProxyServers.Add(proxyServer);
            }
        }

        [RelayCommand]
        private async Task AddProxyServerAsync()
        {
            var viewModel = _dialogFacade.ShowDialog<AddProxyServerViewModel>(this);

            if (viewModel.ProxyServer is not null)
            {
                var proxy = await _proxyService.AddAsync(viewModel.ProxyServer, CancellationToken.None);
                ProxyServers.Add(proxy);
            }
        }

        [RelayCommand]
        private async Task EditProxyServerAsync(ProxyListDto proxyServer)
        {
            var proxyDetail = await _proxyService.GetByIdAsync(proxyServer.Id, CancellationToken.None);
            if (proxyDetail is null)
            {
                return;
            }

            var viewModel = new EditProxyServerViewModel(proxyDetail);

            _dialogFacade.ShowDialog(this, viewModel);

            if (viewModel.ProxyServer is not null)
            {
                var proxy = await _proxyService.UpdateAsync(viewModel.ProxyServer, CancellationToken.None);
                if (proxy is not null)
                {
                    var index = ProxyServers.IndexOf(proxyServer);
                    if (index >= 0)
                    {
                        ProxyServers[index] = proxy;
                    }
                }
            }
        }

        [RelayCommand]
        private async Task RemoveProxyServerAsync(ProxyListDto proxyServer)
        {
            await _proxyService.DeleteAsync(proxyServer.Id, CancellationToken.None);
            ProxyServers.Remove(proxyServer);
        }

        [RelayCommand]
        private async Task ConfirmAsync()
        {
            await _proxyService.SaveChangesAsync(CancellationToken.None);

            DialogResult = true;
        }
    }
}
