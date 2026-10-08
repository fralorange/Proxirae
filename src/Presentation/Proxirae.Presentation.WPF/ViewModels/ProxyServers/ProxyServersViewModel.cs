using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using MvvmDialogs;
using Proxirae.Application.Security.Protection;
using Proxirae.Application.Services.Proxies;
using Proxirae.Application.Validators.ProxyRule;
using Proxirae.Contracts.DTOs.Proxies;
using Proxirae.Presentation.WPF.Factories.ProxyChecker;
using Proxirae.Presentation.WPF.Services.Dialog.Message;
using Proxirae.Presentation.WPF.Services.Dialog.Modal;
using System.Collections.ObjectModel;

namespace Proxirae.Presentation.WPF.ViewModels.ProxyServers
{
    public partial class ProxyServersViewModel : ObservableObject, IModalDialogViewModel
    {
        private readonly IModalDialogService _modalDialogService;
        private readonly IMessageDialogService _messageDialogService;
        private readonly IProxyService _proxyService;
        private readonly IProxyCheckerViewModelFactory _proxyCheckerFactory;
        private readonly IProxyRuleValidator _proxyRuleValidator;
        private readonly IProtector _protector;

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

        public ProxyServersViewModel(IModalDialogService dialogFacade,
                                     IMessageDialogService messageDialogService,
                                     IProxyService proxyService,
                                     IProxyCheckerViewModelFactory proxyCheckerFactory,
                                     IProxyRuleValidator proxyRuleValidator,
                                     IProtector protector)
        {
            _modalDialogService = dialogFacade;
            _messageDialogService = messageDialogService;
            _proxyService = proxyService;
            _proxyCheckerFactory = proxyCheckerFactory;
            _proxyRuleValidator = proxyRuleValidator;
            _protector = protector;
        }

        [RelayCommand]
        private async Task LoadAsync(CancellationToken cancellationToken)
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
            var viewModel = _modalDialogService.ShowDialog<AddProxyServerViewModel>(this);

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

            var viewModel = new EditProxyServerViewModel(_protector, proxyDetail);

            _modalDialogService.ShowDialog(this, viewModel);

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
            var affectedRules = await _proxyRuleValidator.GetAffectedRulesAsync(proxyServer.Id, cancellationToken);
            if (affectedRules.Count > 0 && !_messageDialogService.ShowWarning(this, "ProxyDelete", "ProxyDeleteTitle"))
            {
                return;
            }

            await _proxyService.DeleteAsync(proxyServer.Id, cancellationToken);
            await _proxyRuleValidator.UnbindProxyFromRulesAsync(proxyServer.Id, cancellationToken); 

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
            _modalDialogService.ShowDialog(this, viewModel);
        }

        [RelayCommand]
        private async Task ConfirmAsync(CancellationToken cancellationToken)
        {
            await _proxyService.SaveChangesAsync(cancellationToken);
            await _proxyRuleValidator.SaveChangesAsync(cancellationToken);

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
