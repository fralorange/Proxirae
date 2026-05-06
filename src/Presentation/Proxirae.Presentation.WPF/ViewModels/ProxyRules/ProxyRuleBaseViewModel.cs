using CommunityToolkit.Mvvm.ComponentModel;
using MvvmDialogs;
using Proxirae.Contracts.DTOs.Rules;
using Proxirae.Contracts.DTOs.Rules.Actions;
using System.Collections.ObjectModel;
using System.ComponentModel.DataAnnotations;

namespace Proxirae.Presentation.WPF.ViewModels.ProxyRules
{
    public partial class ProxyRuleBaseViewModel : ObservableValidator, IModalDialogViewModel
    {
        private bool? _dialogResult;

        public bool? DialogResult
        {
            get => _dialogResult;
            protected set => SetProperty(ref _dialogResult, value);
        }

        public List<RuleProtocolDto> Protocols { get; } =
            Enum.GetValues<RuleProtocolDto>()
                .Where(p => p != RuleProtocolDto.None)
                .ToList();

        [ObservableProperty]
        private List<BaseActionDto> _actions;

        [ObservableProperty]
        [CustomValidation(typeof(ProxyRuleBaseViewModel), nameof(ValidateSelectedProtocols))]
        private ObservableCollection<RuleProtocolDto> _selectedProtocols = [RuleProtocolDto.TCP];

        [ObservableProperty]
        private string? _remarks;

        [ObservableProperty]
        private string? _processes;

        [ObservableProperty]
        private string? _hosts;

        [ObservableProperty]
        private string? _ports;

        [ObservableProperty]
        [Required]
        private BaseActionDto? _selectedAction;

        [ObservableProperty]
        private bool _isEnabled = true;

        public ProxyRuleBaseViewModel(List<BaseActionDto> actions)
        {
            Actions = actions;
            SelectedProtocols.CollectionChanged += (_, _) => ValidateProperty(SelectedProtocols, nameof(SelectedProtocols));
            SelectedAction = Actions.First();
        }

        public static ValidationResult? ValidateSelectedProtocols(ObservableCollection<RuleProtocolDto> protocols, ValidationContext context)
        {
            if (protocols == null || protocols.Count == 0)
                return new ValidationResult("The protocols list must not be empty");

            return ValidationResult.Success;
        }
    }
}
