using CommunityToolkit.Mvvm.Input;
using Proxirae.Application.Factories.Rule;
using Proxirae.Contracts.DTOs.Rules;
using Proxirae.Contracts.DTOs.Rules.Actions;
using Proxirae.Presentation.WPF.Facades.Dialog;

namespace Proxirae.Presentation.WPF.ViewModels.ProxyRules
{
    public partial class EditProxyRuleViewModel : ProxyRuleBaseViewModel
    {
        private readonly Guid _id;

        public RuleEditDto? ProxyRule { get; private set; }

        public EditProxyRuleViewModel(DialogFacade dialogFacade, List<BaseActionDto> actions, RuleDto ruleDto) 
            : base(dialogFacade, actions)
        {
            _id = ruleDto.Id;
            Priority = ruleDto.Priority;
            IsEnabled = ruleDto.IsEnabled;
            Remarks = ruleDto.Remarks;
            Processes = ruleDto.Processes;
            Hosts = ruleDto.Hosts;
            Ports = ruleDto.Ports;
            SelectedProtocols = new(Enum.GetValues(typeof(RuleProtocolDto))
                .Cast<RuleProtocolDto>()
                .Where(p => p != RuleProtocolDto.None && ruleDto.Protocol.HasFlag(p)));
            SelectedAction = Actions.FirstOrDefault(a =>
            {
                if (a is ProxyActionDto proxyAction && ruleDto.Action is ProxyActionDto ruleAction)
                    return proxyAction.Id == ruleAction.Id;

                return a.Name == ruleDto.Action.Name;
            });
        }

        [RelayCommand]
        private void Confirm()
        {
            ValidateAllProperties();

            if (HasErrors)
                return;

            ProxyRule = RuleFactory.CreateEditDto(_id, Priority, IsEnabled, Remarks, Processes, Hosts, Ports, SelectedProtocols, SelectedAction!);

            DialogResult = true;
        }
    }
}
