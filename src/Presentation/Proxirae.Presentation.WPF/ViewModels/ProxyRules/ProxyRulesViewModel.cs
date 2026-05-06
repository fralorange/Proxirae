using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using MvvmDialogs;
using Proxirae.Application.Facades.Actions;
using Proxirae.Application.Services.Rules;
using Proxirae.Contracts.DTOs.Rules;
using Proxirae.Contracts.DTOs.Rules.Actions;
using Proxirae.Presentation.WPF.Facades.Dialog;
using System.Collections.ObjectModel;

namespace Proxirae.Presentation.WPF.ViewModels.ProxyRules
{
    public partial class ProxyRulesViewModel : ObservableObject, IModalDialogViewModel
    {
        private readonly DialogFacade _dialogFacade;
        private readonly IRuleService _ruleService;
        private readonly ActionFacade _actionFacade;

        private bool? dialogResult;

        public bool? DialogResult
        {
            get => dialogResult;
            private set => SetProperty(ref dialogResult, value);
        }

        public ObservableCollection<RuleDto> ProxyRules { get; } = [];

        [ObservableProperty]
        private List<BaseActionDto> _actions = [];

        public ProxyRulesViewModel(DialogFacade dialogFacade, IRuleService ruleService, ActionFacade actionFacade)
        {
            _dialogFacade = dialogFacade;
            _ruleService = ruleService;
            _actionFacade = actionFacade;
        }

        [RelayCommand]
        private async Task LoadAsync()
        {
            Actions = await _actionFacade.GetActionsAsync(CancellationToken.None);
            var proxyRules = await _ruleService.GetAsync(CancellationToken.None);

            ProxyRules.Clear();

            foreach (var proxyRule in proxyRules)
            {
                ProxyRules.Add(proxyRule);
            }
        }

        [RelayCommand]
        private async Task AddProxyRuleAsync()
        {
            var viewModel = new AddProxyRuleViewModel(Actions);
            _dialogFacade.ShowDialog(this, viewModel);

            if (viewModel.ProxyRule is { } addRule)
            {
                var rule = await _ruleService.AddAsync(addRule, CancellationToken.None);
                ProxyRules.Add(rule);
            }
        }

        [RelayCommand]
        private async Task EditProxyRuleAsync(RuleDto proxyRule)
        {
            var viewModel = new EditProxyRuleViewModel(Actions, proxyRule);
            _dialogFacade.ShowDialog(this, viewModel);

            if (viewModel.ProxyRule is { } editRule)
            {
                var rule = await _ruleService.UpdateAsync(editRule, CancellationToken.None);
                if (rule is not null)
                {
                    var index = ProxyRules.IndexOf(proxyRule);
                    if (index >= 0)
                    {
                        ProxyRules[index] = rule;
                    }
                }
            }
        }

        [RelayCommand]
        private async Task UpdateProxyRuleAsync(RuleDto proxyRule)
        {
            var viewModel = new EditProxyRuleViewModel(Actions, proxyRule);
            // TODO: Remove the use of ConfirmCommand outside the UI
            viewModel.ConfirmCommand.Execute(CancellationToken.None);

            if (viewModel.ProxyRule is { } editRule)
            {
                var rule = await _ruleService.UpdateAsync(editRule, CancellationToken.None);
                if (rule is not null)
                {
                    var index = ProxyRules.IndexOf(proxyRule);
                    if (index >= 0)
                    {
                        ProxyRules[index] = rule;
                    }
                }
            }
        }

        [RelayCommand]
        private async Task DeleteProxyRuleAsync(RuleDto proxyRule)
        {
            await _ruleService.DeleteAsync(proxyRule.Id, CancellationToken.None);
            ProxyRules.Remove(proxyRule);
        }

        [RelayCommand]
        private async Task ConfirmAsync()
        {
            await _ruleService.SaveChangesAsync(CancellationToken.None);

            DialogResult = true;
        }


    }
}
