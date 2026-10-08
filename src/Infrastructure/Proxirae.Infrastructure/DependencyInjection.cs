using Microsoft.Extensions.DependencyInjection;
using Proxirae.Application.Exporters.Csv;
using Proxirae.Application.Persistence;
using Proxirae.Application.Repositories;
using Proxirae.Application.Security.Cryptography;
using Proxirae.Application.Security.Protection;
using Proxirae.Application.Services.Configuration;
using Proxirae.Application.Stores.Preferences;
using Proxirae.Contracts.Data.Rules;
using Proxirae.Domain.Proxies;
using Proxirae.Infrastructure.Exporters.Csv;
using Proxirae.Infrastructure.Persistence;
using Proxirae.Infrastructure.Repositories.Proxies;
using Proxirae.Infrastructure.Repositories.Rules;
using Proxirae.Infrastructure.Security.Cryptography;
using Proxirae.Infrastructure.Security.Protection.Windows;
using Proxirae.Infrastructure.Services.Configuration;
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

        public static IServiceCollection AddExporters(this IServiceCollection services)
        {
            services.AddTransient<ICsvExporter, SepCsvExporter>();

            return services;
        }

        public static IServiceCollection AddSecurity(this IServiceCollection services)
        {
            services.AddTransient<ICryptographer, AesCryptographer>();

            if (OperatingSystem.IsWindows())
            {
                services.AddTransient<IProtector, WindowsProtector>();
            }

            return services;
        }
    }
}
