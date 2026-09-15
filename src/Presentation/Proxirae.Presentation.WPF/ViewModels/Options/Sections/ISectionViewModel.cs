using AppPreferences = Proxirae.Application.Models.Preferences.Preferences;

namespace Proxirae.Presentation.WPF.ViewModels.Options.Sections
{
    public interface ISectionViewModel
    {
        bool HasChanges { get; set; }
        bool Validate();
        AppPreferences ApplyChanges(AppPreferences current);
    }
}
