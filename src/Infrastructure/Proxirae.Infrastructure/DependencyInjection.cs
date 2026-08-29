using Microsoft.Extensions.DependencyInjection;
using Proxirae.Application.Repositories;
using Proxirae.Application.Stores;
using Proxirae.Application.Stores.Preferences;
using Proxirae.Contracts.Data.Rules;
using Proxirae.Domain.Proxies;
using Proxirae.Infrastructure.Persistence;
using Proxirae.Infrastructure.Repositories.Proxies;
using Proxirae.Infrastructure.Repositories.Rules;
using Proxirae.Infrastructure.Stores.Preferences;

namespace Proxirae.Infrastructure
{
    public static class DependencyInjection
    {
        public static IServiceCollection AddRepositories(this IServiceCollection services)
        {
            var directory = Path.Combine(
                Environment.GetFolderPath(
                    Environment.SpecialFolder.ApplicationData),
                nameof(Proxirae));

            Directory.CreateDirectory(directory);

            services.AddSingleton(new PersistenceOptions
            {
                Directory = directory
            });

            services.AddSingleton(sp =>
            {
                var options = sp.GetRequiredService<PersistenceOptions>();

                return new JsonProxyRepository(options.ProxiesFile);
            });

            services.AddSingleton<ICrudRepository<Proxy>>(sp =>
                sp.GetRequiredService<JsonProxyRepository>());

            services.AddSingleton<IPersistence<Proxy>>(sp =>
                sp.GetRequiredService<JsonProxyRepository>());

            services.AddSingleton(sp =>
            {
                var options = sp.GetRequiredService<PersistenceOptions>();

                return new JsonRuleRepository(options.RulesFile);
            });

            services.AddSingleton<ICrudRepository<RuleData>>(sp =>
                sp.GetRequiredService<JsonRuleRepository>());

            services.AddSingleton<IPersistence<RuleData>>(sp =>
                sp.GetRequiredService<JsonRuleRepository>());

            return services;
        }

        public static IServiceCollection AddStores(this IServiceCollection services)
        {
            services.AddSingleton(sp =>
            {
                var options = sp.GetRequiredService<PersistenceOptions>();
                return new PreferencesStore(options.PreferencesFile);
            });

            services.AddSingleton<IPreferencesStore>(sp =>
                sp.GetRequiredService<PreferencesStore>());

            return services;
        }
    }
}
