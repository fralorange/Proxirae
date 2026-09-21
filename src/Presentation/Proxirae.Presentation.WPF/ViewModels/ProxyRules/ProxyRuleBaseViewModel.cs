using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using MvvmDialogs;
using MvvmDialogs.FrameworkDialogs.OpenFile;
using Proxirae.Contracts.DTOs.Rules;
using Proxirae.Contracts.DTOs.Rules.Actions;
using Proxirae.Presentation.WPF.Services.Dialog.File;
using Proxirae.Presentation.WPF.Services.Dialog.Modal;
using System.Collections.ObjectModel;
using System.ComponentModel.DataAnnotations;
using System.IO;

namespace Proxirae.Presentation.WPF.ViewModels.ProxyRules
{
    public partial class ProxyRuleBaseViewModel : ObservableValidator, IModalDialogViewModel, IDisposable
    {
        private readonly IFileDialogService _fileDialogService;

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

        [ObservableProperty]
        private int _priority;

        public ProxyRuleBaseViewModel(IFileDialogService fileDialogService, List<BaseActionDto> actions)
        {
            _fileDialogService = fileDialogService;

            Actions = actions;
            SelectedProtocols.CollectionChanged += OnSelectedProtocolsCollectionChanged;
            SelectedAction = Actions.First();
        }

        private void OnSelectedProtocolsCollectionChanged(object? sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            ValidateProperty(SelectedProtocols, nameof(SelectedProtocols));
        }

        [RelayCommand]
        private void BrowseProcesses()
        {
            var settings = new OpenFileDialogSettings
            {
                Title = "Open File",
                InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments),
                Filter = "Executable Files (*.exe)|*.exe|All Files (*.*)|*.*"
            };

            var filePath = _fileDialogService.OpenFile(this, settings);
            if (filePath is null)
            {
                return;
            }

            var fileName = Path.GetFileName(filePath);

            if (fileName.Contains(' '))
            {
                fileName = $"\"{fileName}\"";
            }

            if (string.IsNullOrWhiteSpace(Processes))
            {
                Processes = fileName;
            }
            else
            {
                Processes += $"; {fileName}";
            }
        }

        public static ValidationResult? ValidateSelectedProtocols(ObservableCollection<RuleProtocolDto> protocols, ValidationContext context)
        {
            if (protocols == null || protocols.Count == 0)
                return new ValidationResult("The protocols list must not be empty");

            return ValidationResult.Success;
        }

        public void Dispose()
        {
            SelectedProtocols.CollectionChanged -= OnSelectedProtocolsCollectionChanged;
        }
    }
}
