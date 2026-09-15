using CommunityToolkit.Mvvm.ComponentModel;
using Proxirae.Application.Models.Preferences;
using Proxirae.Application.Services.Preferences;
using Proxirae.Presentation.WPF.Models.Themes;

namespace Proxirae.Presentation.WPF.ViewModels.Options.Sections
{
    public partial class AppearanceViewModel : BaseSectionViewModel
    {
        public List<Theme> Themes { get; set; }

        [ObservableProperty]
        [NotifyPropertyChangedFor(nameof(HasChanges))]
        private Theme _selectedTheme;

        public AppearanceViewModel(IPreferencesService preferencesService)
        {
            Themes =
            [
                new Theme { Name = "System" },
                new Theme { Name = "Light" },
                new Theme { Name = "Dark" },
            ];

            SelectedTheme = Themes.First(th => preferencesService.Current.Appearance.Theme == th.Name);

            HasChanges = false;
        }

        public override Preferences ApplyChanges(Preferences current)
        {
            return current with
            {
                Appearance = current.Appearance with
                {
                    Theme = SelectedTheme.Name,
                }
            };
        }

        partial void OnSelectedThemeChanged(Theme value)
        {
            HasChanges = true;
        }
    }
}
