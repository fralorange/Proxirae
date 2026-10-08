using CommunityToolkit.Mvvm.ComponentModel;
using Proxirae.Application.Models.Preferences;
using Proxirae.Application.Services.Preferences;
using Proxirae.Presentation.WPF.Models.Language;

namespace Proxirae.Presentation.WPF.ViewModels.Options.Sections
{
    public partial class GeneralViewModel : BaseSectionViewModel
    {
        public List<Language> Languages { get; }

        [ObservableProperty]
        [NotifyPropertyChangedFor(nameof(HasChanges))]
        private Language _selectedLanguage;

        [ObservableProperty]
        [NotifyPropertyChangedFor(nameof(HasChanges))]
        private bool _autostart;

        [ObservableProperty]
        [NotifyPropertyChangedFor(nameof(HasChanges))]
        private bool _silentStart;

        [ObservableProperty]
        [NotifyPropertyChangedFor(nameof(HasChanges))]
        private bool _startMinimized;

        public GeneralViewModel(IPreferencesService preferencesService)
        {
            Languages =
            [
                new Language { Code = "en-US", Name = "English" },
                new Language { Code = "ru-RU", Name = "Russian" },
            ];

            var system = preferencesService.Current.System;

            SelectedLanguage = Languages.First(
                language => language.Code == system.LanguageCode);

            Autostart = system.Autostart;
            SilentStart = system.SilentStart;
            StartMinimized = system.StartMinimized;

            NormalizeStartupOptions();

            HasChanges = false;
        }

        public override Preferences ApplyChanges(Preferences current)
        {
            return current with
            {
                System = current.System with
                {
                    LanguageCode = SelectedLanguage.Code,
                    Autostart = Autostart,
                    SilentStart = SilentStart,
                    StartMinimized = StartMinimized,
                }
            };
        }

        partial void OnSelectedLanguageChanged(Language value)
        {
            HasChanges = true;
        }

        partial void OnAutostartChanged(bool value)
        {
            if (!value)
            {
                SilentStart = false;
                StartMinimized = false;
            }

            HasChanges = true;
        }

        partial void OnSilentStartChanged(bool value)
        {
            if (value)
            {
                StartMinimized = false;
            }

            HasChanges = true;
        }

        partial void OnStartMinimizedChanged(bool value)
        {
            if (value)
            {
                SilentStart = false;
            }

            HasChanges = true;
        }

        private void NormalizeStartupOptions()
        {
            if (!Autostart)
            {
                SilentStart = false;
                StartMinimized = false;
            }
            else if (SilentStart)
            {
                StartMinimized = false;
            }
        }
    }
}