using Proxirae.Application.Facades.Configuration;
using Proxirae.Application.Messenger;
using Proxirae.Application.Stores;
using Proxirae.Contracts.Data.Rules;
using Proxirae.Domain.Proxies;
using Proxirae.Infrastructure.Persistence;

namespace Proxirae.Infrastructure.Facades
{
    public class ConfigurationFacade : IConfigurationFacade
    {
        private readonly PersistenceOptions _persistenceOptions;
        private readonly IPersistence<Proxy> _proxyPersistence;
        private readonly IPersistence<RuleData> _rulePersistence;
        private readonly IMessenger _messenger;

        public ConfigurationFacade(PersistenceOptions persistenceOptions,
                                   IPersistence<Proxy> proxyPersistence,
                                   IPersistence<RuleData> rulePersistence,
                                   IMessenger messenger)
        {
            _persistenceOptions = persistenceOptions;
            _proxyPersistence = proxyPersistence;
            _rulePersistence = rulePersistence;
            _messenger = messenger;
        }

        public string AppDataDirectory => _persistenceOptions.Directory;
        public string[] ConfigurationFiles => [_persistenceOptions.RulesFile, _persistenceOptions.ProxiesFile];

        public async Task ReloadAsync(CancellationToken cancellationToken)
        {
            await _proxyPersistence.ReloadAsync(cancellationToken);
            await _rulePersistence.ReloadAsync(cancellationToken);

            await _messenger.SendMessageAsync(MessageType.Cmd_ReloadProxies, cancellationToken);
            await _messenger.SendMessageAsync(MessageType.Cmd_ReloadRules, cancellationToken);
        }
    }
}
