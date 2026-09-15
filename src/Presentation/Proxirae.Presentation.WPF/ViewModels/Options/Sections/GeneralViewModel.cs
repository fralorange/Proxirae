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

        public GeneralViewModel(IPreferencesService preferencesService)
        {
            Languages =
            [
                new Language { Code = "en-US", Name = "English" },
                new Language { Code = "ru-RU", Name = "Russian" },
            ];

            SelectedLanguage = Languages.First(l => l.Code == preferencesService.Current.System.LanguageCode);

            HasChanges = false;
        }

        public override Preferences ApplyChanges(Preferences current)
        {
            return current with
            {
                System = current.System with
                {
                    LanguageCode = SelectedLanguage.Code,
                }
            };
        }

        partial void OnSelectedLanguageChanged(Language value)
        {
            HasChanges = true;
        }
    }
}
