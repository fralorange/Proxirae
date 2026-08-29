using Proxirae.Application.Models.Preferences.Appearance;
using Proxirae.Application.Models.Preferences.Engine;
using Proxirae.Application.Models.Preferences.System;
using Proxirae.Application.Stores.Preferences;
using Proxirae.Infrastructure.Persistence.JSON;

using AppPreferences = Proxirae.Application.Models.Preferences.Preferences;

namespace Proxirae.Infrastructure.Stores.Preferences
{
    public class PreferencesStore : JsonSinglePersistence<AppPreferences>, IPreferencesStore
    {
        public AppPreferences Current => _item;

        public PreferencesStore(string filePath) : base(filePath) { }

        public Task LoadAsync(CancellationToken cancellationToken)
        {
            return EnsureLoadedAsync(cancellationToken);
        }

        public async Task UpdateAsync(SystemPreferences preferences, CancellationToken cancellationToken)
        {
            await EnsureLoadedAsync(cancellationToken);

            _item = _item with { System = preferences };

            await SaveAsync(cancellationToken);
        }

        public async Task UpdateAsync(EnginePreferences preferences, CancellationToken cancellationToken)
        {
            await EnsureLoadedAsync(cancellationToken);

            _item = _item with { Engine = preferences };

            await SaveAsync(cancellationToken);
        }

        public async Task UpdateAsync(AppearancePreferences preferences, CancellationToken cancellationToken)
        {
            await EnsureLoadedAsync(cancellationToken);

            _item = _item with { Appearance = preferences };

            await SaveAsync(cancellationToken);
        }
    }
}
