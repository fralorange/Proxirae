using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using MvvmDialogs;
using Proxirae.Application.Facades.Actions;
using Proxirae.Application.Factories.Rule;
using Proxirae.Application.Services.Rules;
using Proxirae.Contracts.DTOs.Rules;
using Proxirae.Contracts.DTOs.Rules.Actions;
using Proxirae.Presentation.WPF.Services.Dialog.File;
using Proxirae.Presentation.WPF.Services.Dialog.Modal;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Windows.Data;

namespace Proxirae.Presentation.WPF.ViewModels.ProxyRules
{
    public partial class ProxyRulesViewModel : ObservableObject, IModalDialogViewModel
    {
        private readonly IModalDialogService _modalDialogService;
        private readonly IFileDialogService _fileDialogService;
        private readonly IRuleService _ruleService;
        private readonly ActionFacade _actionFacade;

        private bool? dialogResult;

        public bool? DialogResult
        {
            get => dialogResult;
            private set => SetProperty(ref dialogResult, value);
        }

        private ObservableCollection<RuleDto> _proxyRules { get; } = [];
        public ICollectionView ProxyRules { get; }

        [ObservableProperty]
        private List<BaseActionDto> _actions = [];

        [ObservableProperty]
        [NotifyCanExecuteChangedFor(nameof(ApplyCommand))]
        private bool _hasChanges;

        public ProxyRulesViewModel(IModalDialogService modalDialogService, IFileDialogService fileDialogService, IRuleService ruleService, ActionFacade actionFacade)
        {
            _modalDialogService = modalDialogService;
            _fileDialogService = fileDialogService;
            _ruleService = ruleService;
            _actionFacade = actionFacade;

            ProxyRules = CollectionViewSource.GetDefaultView(_proxyRules);
            ProxyRules.SortDescriptions.Clear();

            ProxyRules.SortDescriptions.Add(
                new SortDescription(nameof(RuleDto.Priority), ListSortDirection.Descending)
            );
        }

        [RelayCommand]
        private async Task LoadAsync(CancellationToken cancellationToken)
        {
            Actions = await _actionFacade.GetActionsAsync(cancellationToken);
            var proxyRules = await _ruleService.GetAsync(cancellationToken);

            _proxyRules.Clear();

            foreach (var proxyRule in proxyRules)
            {
                _proxyRules.Add(proxyRule);
            }

            _proxyRules.CollectionChanged += (_, _) => HasChanges = true;
        }

        [RelayCommand]
        private async Task AddProxyRuleAsync(CancellationToken cancellationToken)
        {
            var nextPriority = 1;
            if (_proxyRules.Count > 0)
            {
                nextPriority = _proxyRules.Max(r => r.Priority) + 1;
            }

            var viewModel = new AddProxyRuleViewModel(_fileDialogService, Actions, nextPriority);
            _modalDialogService.ShowDialog(this, viewModel);

            if (viewModel.ProxyRule is { } addRule)
            {
                var rule = await _ruleService.AddAsync(addRule, cancellationToken);
                _proxyRules.Add(rule);
            }
        }

        [RelayCommand]
        private async Task EditProxyRuleAsync(RuleDto proxyRule, CancellationToken cancellationToken)
        {
            var viewModel = new EditProxyRuleViewModel(_fileDialogService, Actions, proxyRule);
            _modalDialogService.ShowDialog(this, viewModel);

            if (viewModel.ProxyRule is { } editRule)
            {
                var rule = await _ruleService.UpdateAsync(editRule, cancellationToken);
                if (rule is not null)
                {
                    var index = _proxyRules.IndexOf(proxyRule);
                    if (index >= 0)
                    {
                        _proxyRules[index] = rule;
                    }
                }
            }
        }

        [RelayCommand]
        private async Task UpdateProxyRuleAsync(RuleDto proxyRule, CancellationToken cancellationToken)
        {
            var editRule = RuleFactory.CreateEditDto(proxyRule);

            if (editRule is not null)
            {
                var rule = await _ruleService.UpdateAsync(editRule, cancellationToken);
                if (rule is not null)
                {
                    var index = _proxyRules.IndexOf(proxyRule);
                    if (index >= 0)
                    {
                        _proxyRules[index] = rule;
                    }
                }
            }
        }

        [RelayCommand]
        private async Task DeleteProxyRuleAsync(RuleDto proxyRule, CancellationToken cancellationToken)
        {
            await _ruleService.DeleteAsync(proxyRule.Id, cancellationToken);
            _proxyRules.Remove(proxyRule);
        }

        [RelayCommand]
        private async Task MoveProxyRuleUpAsync(RuleDto proxyRule, CancellationToken cancellationToken)
        {
            var nextRule = _proxyRules
                .Where(r => proxyRule.Priority < r.Priority)
                .OrderBy(r => r.Priority)
                .FirstOrDefault();

            if (nextRule is not null)
            {
                (proxyRule.Priority, nextRule.Priority) = (nextRule.Priority, proxyRule.Priority);

                ProxyRules.Refresh();

                await _ruleService.SwapPrioritiesAsync(proxyRule.Id, nextRule.Id, cancellationToken);
            }
        }

        [RelayCommand]
        private async Task MoveProxyRuleDownAsync(RuleDto proxyRule, CancellationToken cancellationToken)
        {
            var previousRule = _proxyRules
                .Where(r => r.Priority < proxyRule.Priority)
                .OrderByDescending(r => r.Priority)
                .FirstOrDefault();

            if (previousRule is not null)
            {
                (proxyRule.Priority, previousRule.Priority) = (previousRule.Priority, proxyRule.Priority);

                ProxyRules.Refresh();

                await _ruleService.SwapPrioritiesAsync(proxyRule.Id, previousRule.Id, cancellationToken);
            }
        }

        [RelayCommand]
        private async Task ConfirmAsync(CancellationToken cancellationToken)
        {
            await _ruleService.SaveChangesAsync(cancellationToken);

            DialogResult = true;
        }

        private bool IsApplyExecutable() { return HasChanges; }

        [RelayCommand(CanExecute = nameof(IsApplyExecutable))]
        private async Task ApplyAsync(CancellationToken cancellationToken)
        {
            if (HasChanges)
            {
                await _ruleService.SaveChangesAsync(cancellationToken);
                HasChanges = false;
            }
        }
    }
}
