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

        public event EventHandler<AppPreferences>? PreferencesChanged;

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

        public async Task UpdateAsync<T>(T preferences, CancellationToken cancellationToken = default)
        {
            await _preferencesStore.UpdateAsync(current => preferences switch
            {
                SystemPreferences sys => current with { System = sys },
                EnginePreferences eng => current with { Engine = eng },
                AppearancePreferences app => current with { Appearance = app },
                AppPreferences full => full,
                _ => throw new ArgumentOutOfRangeException(nameof(preferences), $"Unsupported preferences type: {typeof(T).Name}")
            }, cancellationToken);

            PreferencesChanged?.Invoke(this, Current);
        }

        public async Task UpdateAsync(EnginePreferences preferences, CancellationToken cancellationToken = default)
        {
            await UpdateAsync<EnginePreferences>(preferences, cancellationToken);
            await _messenger.SendMessageAsync(MessageType.Cmd_ReloadPreferences, cancellationToken);
        }
    }
}
