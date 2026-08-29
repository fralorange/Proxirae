using Proxirae.Application.Models.Preferences.Appearance;
using Proxirae.Application.Models.Preferences.Engine;
using Proxirae.Application.Models.Preferences.System;

using AppPreferences = Proxirae.Application.Models.Preferences.Preferences;

namespace Proxirae.Application.Stores.Preferences
{
    public interface IPreferencesStore
    {
        AppPreferences Current { get; }
        Task LoadAsync(CancellationToken cancellationToken);
        Task UpdateAsync(SystemPreferences preferences, CancellationToken cancellationToken);
        Task UpdateAsync(EnginePreferences preferences, CancellationToken cancellationToken);
        Task UpdateAsync(AppearancePreferences preferences, CancellationToken cancellationToken);
    }
}
