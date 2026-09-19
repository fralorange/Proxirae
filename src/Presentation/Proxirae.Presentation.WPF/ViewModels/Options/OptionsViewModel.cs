using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using MvvmDialogs;
using Proxirae.Application.Services.Localization;
using Proxirae.Application.Services.Preferences;
using Proxirae.Presentation.WPF.Models.Sections;
using Proxirae.Presentation.WPF.Services.Themes;
using Proxirae.Presentation.WPF.ViewModels.Options.Sections;

namespace Proxirae.Presentation.WPF.ViewModels.Options
{
    public partial class OptionsViewModel : ObservableObject, IModalDialogViewModel
    {
        private readonly IPreferencesService _preferencesService;
        private readonly ILocalizationService _localizationService;
        private readonly IThemeService _themeService;  

        private bool? _dialogResult;
        public bool? DialogResult
        {
            get => _dialogResult;
            protected set => SetProperty(ref _dialogResult, value);
        }

        public List<Section> Sections { get; }

        [ObservableProperty]
        private ISectionViewModel _selectedSection;

        public OptionsViewModel(IPreferencesService preferencesService, GeneralViewModel generalViewModel, AppearanceViewModel appearanceViewModel, MetricsViewModel metricsViewModel, ILocalizationService localizationService, IThemeService themeService)
        {
            _preferencesService = preferencesService;
            _localizationService = localizationService;
            _themeService = themeService;

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
        private async Task<bool> ApplyAsync(CancellationToken cancellationToken)
        {
            foreach (var section in Sections)
            {
                if (section.ViewModel is ISectionViewModel sectionViewModel && !sectionViewModel.Validate())
                {
                    SelectedSection = sectionViewModel;
                    return false;
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

            var languageChanged =
                _preferencesService.Current.System.LanguageCode != newPreferences.System.LanguageCode;

            var themeChanged = 
                _preferencesService.Current.Appearance.Theme != newPreferences.Appearance.Theme;
            // if the number of options increases, this comparison will need to be refactored.

            await _preferencesService.UpdateAsync(newPreferences, cancellationToken);

            if (languageChanged)
            {
                _localizationService.SwitchTo(newPreferences.System.LanguageCode);
            }

            if (themeChanged)
            {
                _themeService.Apply(Enum.Parse<Models.Themes.Theme>(newPreferences.Appearance.Theme));
            }

            foreach (var section in Sections)
            {
                if (section.ViewModel is ISectionViewModel vm)
                {
                    vm.HasChanges = false;
                }
            }

            ApplyCommand.NotifyCanExecuteChanged();

            return true;
        }

        [RelayCommand]
        private async Task ConfirmAsync(CancellationToken cancellationToken)
        {
            var success = await ApplyAsync(cancellationToken);
            if (!success) return;

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
