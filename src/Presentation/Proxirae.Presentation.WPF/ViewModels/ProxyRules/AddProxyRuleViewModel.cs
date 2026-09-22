using CommunityToolkit.Mvvm.Input;
using Proxirae.Application.Factories.Rule;
using Proxirae.Contracts.DTOs.Rules;
using Proxirae.Contracts.DTOs.Rules.Actions;
using Proxirae.Presentation.WPF.Services.Dialog.File;

namespace Proxirae.Presentation.WPF.ViewModels.ProxyRules
{
    public partial class AddProxyRuleViewModel : ProxyRuleBaseViewModel
    {
        public RuleAddDto? ProxyRule { get; private set; }

        public AddProxyRuleViewModel(IFileDialogService fileDialogService, List<BaseActionDto> actions, int nextPriority) : base(fileDialogService, actions)
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
