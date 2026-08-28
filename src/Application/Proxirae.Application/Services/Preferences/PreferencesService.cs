using Proxirae.Application.Messenger;
using Proxirae.Application.Models.Preferences.Appearance;
using Proxirae.Application.Models.Preferences.Engine;
using Proxirae.Application.Models.Preferences.System;
using Proxirae.Application.Stores.Preferences;
using AppPreferences = Proxirae.Application.Models.Preferences.Preferences;

namespace Proxirae.Application.Services.Preferences
{
    public class PreferencesService : IPreferencesService
    {
        public AppPreferences Current => _preferencesStore.Current;

        private readonly IPreferencesStore _preferencesStore;
        private readonly IMessenger _messenger;

        public PreferencesService(IPreferencesStore preferencesStore, IMessenger messenger)
        {
            _preferencesStore = preferencesStore;
            _messenger = messenger;
        }

        public Task LoadAsync(CancellationToken cancellationToken)
        {
            return _preferencesStore.LoadAsync(cancellationToken);
        }

        public Task UpdateAsync(SystemPreferences preferences, CancellationToken cancellationToken)
        {
            return _preferencesStore.UpdateAsync(preferences, cancellationToken);
        }

        public async Task UpdateAsync(EnginePreferences preferences, CancellationToken cancellationToken)
        {
            await _preferencesStore.UpdateAsync(preferences, cancellationToken);
            await _messenger.SendMessageAsync(MessageType.Cmd_ReloadPreferences, cancellationToken);   
        }

        public Task UpdateAsync(AppearancePreferences preferences, CancellationToken cancellationToken)
        {
            return _preferencesStore.UpdateAsync(preferences, cancellationToken);
        }
    }
}
