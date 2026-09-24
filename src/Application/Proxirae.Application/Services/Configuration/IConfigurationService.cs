namespace Proxirae.Application.Services.Configuration
{
    public interface IConfigurationService
    {
        string AppDataDirectory { get; }
        string ProxiesFilePath { get; }
        string RulesFilePath { get; }

        Task ReloadAsync(CancellationToken cancellationToken);
    }
}
