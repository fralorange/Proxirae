using CommunityToolkit.Mvvm.Input;
using Proxirae.Contracts.DTOs.Rules;
using Proxirae.Contracts.DTOs.Rules.Actions;

namespace Proxirae.Presentation.WPF.ViewModels.ProxyRules
{
    public partial class AddProxyRuleViewModel : ProxyRuleBaseViewModel
    {
        public RuleAddDto? ProxyRule { get; private set; }

        public AddProxyRuleViewModel(List<BaseActionDto> actions) : base(actions)
        {
        }

        // TODO: Extract the creation of RuleAddDto to the factory
        [RelayCommand]
        private void Confirm()
        {
            if (HasErrors)
                return;

            ProxyRule = new RuleAddDto
            {
                IsEnabled = IsEnabled,
                Remarks = Remarks,
                Processes = Processes,
                Hosts = Hosts,
                Ports = Ports,
                Protocol = SelectedProtocols.Aggregate((acc, p) => acc | p),
                Action = SelectedAction!,
            };

            DialogResult = true;
        }
    }
}
