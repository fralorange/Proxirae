namespace Proxirae.Infrastructure.Persistence
{
    public sealed class PersistenceOptions
    {
        public string Directory { get; init; } = string.Empty;

        public string ProxiesFile =>
            Path.Combine(Directory, "proxies.json");

        public string RulesFile =>
            Path.Combine(Directory, "rules.json");

        public string PreferencesFile =>
            Path.Combine(Directory, "preferences.json");
    }
}
