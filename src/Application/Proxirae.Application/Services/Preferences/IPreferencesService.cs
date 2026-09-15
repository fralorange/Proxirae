using Proxirae.Application.Models.Preferences.Engine;
using AppPreferences = Proxirae.Application.Models.Preferences.Preferences;

namespace Proxirae.Application.Services.Preferences
{
    public interface IPreferencesService
    {
        AppPreferences Current { get; }

        event EventHandler<AppPreferences>? PreferencesChanged;

        Task LoadAsync(CancellationToken cancellationToken);
        Task UpdateAsync<T>(T preferences, CancellationToken cancellationToken = default);
        Task UpdateAsync(EnginePreferences preferences, CancellationToken cancellationToken = default);
    }
}
