using Proxirae.Application.Messenger;
using Proxirae.Application.Persistence;
using Proxirae.Application.Services.Configuration;
using Proxirae.Contracts.Data.Rules;
using Proxirae.Domain.Proxies;
using Proxirae.Infrastructure.Persistence;

namespace Proxirae.Infrastructure.Services.Configuration
{
    public class ConfigurationService : IConfigurationService
    {
        private readonly PersistenceOptions _persistenceOptions;
        private readonly IPersistence<Proxy> _proxyPersistence;
        private readonly IPersistence<RuleData> _rulePersistence;
        private readonly IMessenger _messenger;

        public string AppDataDirectory => _persistenceOptions.Directory;
        public string ProxiesFilePath => _persistenceOptions.ProxiesFile;
        public string RulesFilePath => _persistenceOptions.RulesFile;

        public ConfigurationService(PersistenceOptions persistenceOptions,
                                   IPersistence<Proxy> proxyPersistence,
                                   IPersistence<RuleData> rulePersistence,
                                   IMessenger messenger)
        {
            _persistenceOptions = persistenceOptions;
            _proxyPersistence = proxyPersistence;
            _rulePersistence = rulePersistence;
            _messenger = messenger;
        }

        public async Task ReloadAsync(CancellationToken cancellationToken)
        {
            await _proxyPersistence.ReloadAsync(cancellationToken);
            await _rulePersistence.ReloadAsync(cancellationToken);

            await _messenger.SendMessageAsync(MessageType.Cmd_ReloadProxies, cancellationToken);
            await _messenger.SendMessageAsync(MessageType.Cmd_ReloadRules, cancellationToken);
        }
    }
}
