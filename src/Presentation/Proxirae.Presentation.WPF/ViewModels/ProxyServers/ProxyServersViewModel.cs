using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using MvvmDialogs;
using Proxirae.Application.Services.Proxies;
using Proxirae.Contracts.DTOs.Proxies;
using Proxirae.Presentation.WPF.Facades.Dialog;
using Proxirae.Presentation.WPF.Factories.ProxyChecker;
using Proxirae.Presentation.WPF.ViewModels.ProxyChecker;
using System.Collections.ObjectModel;

namespace Proxirae.Presentation.WPF.ViewModels.ProxyServers
{
    public partial class ProxyServersViewModel : ObservableObject, IModalDialogViewModel
    {
        private readonly DialogFacade _dialogFacade;
        private readonly IProxyService _proxyService;
        private readonly IProxyCheckerViewModelFactory _proxyCheckerFactory;

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

        public ProxyServersViewModel(DialogFacade dialogFacade, IProxyService proxyService, IProxyCheckerViewModelFactory proxyCheckerFactory)
        {
            _dialogFacade = dialogFacade;
            _proxyService = proxyService;
            _proxyCheckerFactory = proxyCheckerFactory;
        }

        [RelayCommand]
        private async Task LoadProxyServersAsync(CancellationToken cancellationToken)
        {
            var proxyServers = await _proxyService.GetAsync(cancellationToken);

            ProxyServers.Clear();

            foreach (var proxyServer in proxyServers)
            {
                ProxyServers.Add(proxyServer);
            }

            ProxyServers.CollectionChanged += (_, _) => HasChanges = true;
        }

        [RelayCommand]
        private async Task AddProxyServerAsync(CancellationToken cancellationToken)
        {
            var viewModel = _dialogFacade.ShowDialog<AddProxyServerViewModel>(this);

            if (viewModel.ProxyServer is not null)
            {
                var proxy = await _proxyService.AddAsync(viewModel.ProxyServer, cancellationToken);
                ProxyServers.Add(proxy);
            }
        }

        [RelayCommand]
        private async Task EditProxyServerAsync(ProxyListDto proxyServer, CancellationToken cancellationToken)
        {
            var proxyDetail = await _proxyService.GetByIdAsync(proxyServer.Id, cancellationToken);
            if (proxyDetail is null)
            {
                return;
            }

            var viewModel = new EditProxyServerViewModel(proxyDetail);

            _dialogFacade.ShowDialog(this, viewModel);

            if (viewModel.ProxyServer is not null)
            {
                var proxy = await _proxyService.UpdateAsync(viewModel.ProxyServer, cancellationToken);
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
        private async Task RemoveProxyServerAsync(ProxyListDto proxyServer, CancellationToken cancellationToken)
        {
            await _proxyService.DeleteAsync(proxyServer.Id, cancellationToken);
            ProxyServers.Remove(proxyServer);
        }

        [RelayCommand]
        private async Task CheckProxyServerAsync(ProxyListDto proxyServer, CancellationToken cancellationToken)
        {
            var detail = await _proxyService.GetByIdAsync(proxyServer.Id, cancellationToken);
            if (detail is null)
            {
                return;
            }

            var viewModel = _proxyCheckerFactory.Create(detail);
            _dialogFacade.ShowDialog(this, viewModel);
        }

        [RelayCommand]
        private async Task ConfirmAsync(CancellationToken cancellationToken)
        {
            await _proxyService.SaveChangesAsync(cancellationToken);

            DialogResult = true;
        }

        private bool IsApplyExecutable() { return HasChanges; }

        [RelayCommand(CanExecute = nameof(IsApplyExecutable))]
        private async Task ApplyAsync(CancellationToken cancellationToken)
        {
            if (HasChanges)
            {
                await _proxyService.SaveChangesAsync(cancellationToken);
                HasChanges = false;
            }
        }
    }
}
