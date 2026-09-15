using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using Gu.Wpf.NumericInput;
using MvvmDialogs;
using Proxirae.Application.Services.Preferences;
using Proxirae.Presentation.WPF.Models.Sections;
using Proxirae.Presentation.WPF.ViewModels.Options.Sections;

namespace Proxirae.Presentation.WPF.ViewModels.Options
{
    public partial class OptionsViewModel : ObservableObject, IModalDialogViewModel
    {
        private readonly IPreferencesService _preferencesService;

        private bool? _dialogResult;
        public bool? DialogResult
        {
            get => _dialogResult;
            protected set => SetProperty(ref _dialogResult, value);
        }

        public List<Section> Sections { get; }

        [ObservableProperty]
        private ISectionViewModel _selectedSection;

        public OptionsViewModel(IPreferencesService preferencesService, GeneralViewModel generalViewModel, AppearanceViewModel appearanceViewModel, MetricsViewModel metricsViewModel)
        {
            _preferencesService = preferencesService;

            Sections =
            [
                new Section { Title = "General", ViewModel = generalViewModel },
                new Section { Title = "Appearance", ViewModel = appearanceViewModel },
                new Section { Title = "Metrics", ViewModel = metricsViewModel },
            ];

            SelectedSection = generalViewModel;
            SubscribeSections();
        }

        private bool CanApply() => Sections.Any(s => s.ViewModel is ISectionViewModel vm && vm.HasChanges);

        [RelayCommand(CanExecute = nameof(CanApply))]
        private async Task ApplyAsync(CancellationToken cancellationToken)
        {
            foreach (var section in Sections)
            {
                if (section.ViewModel is ISectionViewModel sectionViewModel && !sectionViewModel.Validate())
                {
                    SelectedSection = sectionViewModel;
                    return;
                }
            }

            var newPreferences = _preferencesService.Current;
            foreach (var section in Sections)
            {
                if (section.ViewModel is ISectionViewModel sectionViewModel)
                {
                    newPreferences = sectionViewModel.ApplyChanges(newPreferences);
                }
            }

            await _preferencesService.UpdateAsync(newPreferences, cancellationToken);
        }

        [RelayCommand]
        private async Task ConfirmAsync(CancellationToken cancellationToken)
        {
            await ApplyAsync(cancellationToken);

            DialogResult = true;
        }

        private void SubscribeSections()
        {
            foreach (var section in Sections)
            {
                if (section.ViewModel is BaseSectionViewModel vm)
                {
                    vm.PropertyChanged += (s, e) =>
                    {
                        if (e.PropertyName == nameof(BaseSectionViewModel.HasChanges))
                        {
                            ApplyCommand.NotifyCanExecuteChanged();
                        }
                    };
                }
            }
        }
    }
}
