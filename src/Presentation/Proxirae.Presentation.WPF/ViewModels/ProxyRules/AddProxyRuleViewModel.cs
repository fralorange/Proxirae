using CommunityToolkit.Mvvm.Input;
using Proxirae.Application.Factories.Rule;
using Proxirae.Contracts.DTOs.Rules;
using Proxirae.Contracts.DTOs.Rules.Actions;
using Proxirae.Presentation.WPF.Facades.Dialog;

namespace Proxirae.Presentation.WPF.ViewModels.ProxyRules
{
    public partial class AddProxyRuleViewModel : ProxyRuleBaseViewModel
    {
        public RuleAddDto? ProxyRule { get; private set; }

        public AddProxyRuleViewModel(DialogFacade dialogFacade, List<BaseActionDto> actions, int nextPriority) : base(dialogFacade, actions)
        {
            Priority = nextPriority;
        }

        [RelayCommand]
        private void Confirm()
        {
            ValidateAllProperties();

            if (HasErrors)
                return;

            ProxyRule = RuleFactory.CreateAddDto(Priority, IsEnabled, Remarks, Processes, Hosts, Ports, SelectedProtocols, SelectedAction!);

            DialogResult = true;
        }
    }
}
