using Microsoft.Extensions.Hosting;

namespace Proxirae.Application.Services.Preferences
{
    public class PreferencesHostedService : IHostedService
    {
        private readonly IPreferencesService _preferencesService;

        public PreferencesHostedService(IPreferencesService preferencesService)
        {
            _preferencesService = preferencesService;
        }

        public Task StartAsync(CancellationToken cancellationToken)
        {
            return _preferencesService.LoadAsync(cancellationToken);
        }

        public Task StopAsync(CancellationToken cancellationToken)
        {
            return Task.CompletedTask;
        }
    }
}
