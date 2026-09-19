using CommunityToolkit.Mvvm.ComponentModel;
using Proxirae.Application.Models.Preferences;
using Proxirae.Application.Services.Preferences;
using Proxirae.Presentation.WPF.Models.Themes;

namespace Proxirae.Presentation.WPF.ViewModels.Options.Sections
{
    public partial class AppearanceViewModel : BaseSectionViewModel
    {
        public List<string> Themes { get; } = Enum.GetNames(typeof(Theme)).ToList();

        [ObservableProperty]
        [NotifyPropertyChangedFor(nameof(HasChanges))]
        private string _selectedTheme;

        public AppearanceViewModel(IPreferencesService preferencesService)
        {
            SelectedTheme = preferencesService.Current.Appearance.Theme.ToString();

            HasChanges = false;
        }

        public override Preferences ApplyChanges(Preferences current)
        {
            return current with
            {
                Appearance = current.Appearance with
                {
                    Theme = SelectedTheme,
                }
            };
        }

        partial void OnSelectedThemeChanged(string value)
        {
            HasChanges = true;
        }
    }
}
