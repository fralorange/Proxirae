using AppPreferences = Proxirae.Application.Models.Preferences.Preferences;

namespace Proxirae.Application.Stores.Preferences
{
    public interface IPreferencesStore
    {
        AppPreferences Current { get; }
        Task LoadAsync(CancellationToken cancellationToken);
        Task UpdateAsync(Func<AppPreferences, AppPreferences> update, CancellationToken cancellationToken = default);
    }
}
