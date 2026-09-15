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

        public async Task UpdateAsync(Func<AppPreferences, AppPreferences> update, CancellationToken cancellationToken = default)
        {
            await EnsureLoadedAsync(cancellationToken);

            _item = update(_item);

            await SaveAsync(cancellationToken);
        }
    }
}
