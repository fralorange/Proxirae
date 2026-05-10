using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using MvvmDialogs;
using Proxirae.Application.Services.Proxies;
using Proxirae.Contracts.DTOs.Proxies;
using Proxirae.Presentation.WPF.Facades.Dialog;
using Proxirae.Presentation.WPF.ViewModels.ProxyChecker;
using System.Collections.ObjectModel;

namespace Proxirae.Presentation.WPF.ViewModels.ProxyServers
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

        [ObservableProperty]
        [NotifyCanExecuteChangedFor(nameof(ApplyCommand))]
        private bool _hasChanges;

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

            ProxyServers.CollectionChanged += (_, _) => HasChanges = true;
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
        private async Task CheckProxyServerAsync(ProxyListDto proxyServer)
        {
            var detail = await _proxyService.GetByIdAsync(proxyServer.Id, CancellationToken.None);
            if (detail is null)
            {
                return;
            }

            var viewModel = new ProxyCheckerViewModel(_dialogFacade, detail);
            _dialogFacade.ShowDialog(this, viewModel);
        }

        [RelayCommand]
        private async Task ConfirmAsync()
        {
            await _proxyService.SaveChangesAsync(CancellationToken.None);

            DialogResult = true;
        }

        private bool IsApplyExecutable() { return HasChanges; }

        [RelayCommand(CanExecute = nameof(IsApplyExecutable))]
        private async Task ApplyAsync()
        {
            if (HasChanges)
            {
                await _proxyService.SaveChangesAsync(CancellationToken.None);
                HasChanges = false;
            }
        }
    }
}
